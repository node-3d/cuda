import { join } from 'node:path';
import { tmpdir } from 'node:os';
import {
	Ctx,
	Device,
	getDeviceCount,
	launch,
	memAlloc,
	moduleRuntimeCompile,
} from '@node-3d/cuda';

if (getDeviceCount() === 0) {
	throw new Error('No CUDA devices available');
}

const count = 10;
const arrayA = new Float32Array(count);
const arrayB = new Float32Array(count);
const arrayC = new Float32Array(count);

for (let i = 0; i < count; i++) {
	arrayA[i] = i;
	arrayB[i] = i * 2;
}

const kernelSource = `
extern "C" __global__
void vectorAdd(const float *a, const float *b, float *c, int count) {
	const int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < count) {
		c[i] = a[i] + b[i];
	}
}
`;

const device = new Device(0);
const context = new Ctx(0, device);

const bufferA = memAlloc(arrayA.byteLength);
const bufferB = memAlloc(arrayB.byteLength);
const bufferC = memAlloc(arrayC.byteLength);

const copyAError = bufferA.copyHtoD(arrayA);
const copyBError = bufferB.copyHtoD(arrayB);

if (copyAError !== 0 || copyBError !== 0) {
	throw new Error(`Failed to copy input arrays to device: ${copyAError}, ${copyBError}`);
}

const ptxPath = join(tmpdir(), '@node-3d/cuda-vector-add.ptx').replaceAll('\\', '/');
const module = moduleRuntimeCompile('vector-add.cu', kernelSource, ptxPath);

if (typeof module.error === 'number' && module.error !== 0) {
	throw new Error(`Failed to compile CUDA module: ${module.log ?? 'no compiler log'}`);
}

const vectorAdd = module.getFunction('vectorAdd');

if (typeof vectorAdd.error === 'number' && vectorAdd.error !== 0) {
	throw new Error(`Failed to load vectorAdd kernel: ${vectorAdd.error}`);
}

const launchError = launch(vectorAdd, [1, 1, 1], [count, 1, 1], [
	{ type: 'DevicePtr', value: bufferA.devicePtr },
	{ type: 'DevicePtr', value: bufferB.devicePtr },
	{ type: 'DevicePtr', value: bufferC.devicePtr },
	{ type: 'Int32', value: count },
]);

if (launchError !== 0) {
	throw new Error(`Failed to launch vectorAdd kernel: ${launchError}`);
}

const syncError = context.synchronize();
const copyCError = bufferC.copyDtoH(arrayC);

if (syncError !== 0 || copyCError !== 0) {
	throw new Error(`Failed to read vectorAdd output: ${syncError}, ${copyCError}`);
}

for (let i = 0; i < count; i++) {
	if (arrayC[i] !== arrayA[i] + arrayB[i]) {
		throw new Error(`Unexpected result at ${i}: ${arrayC[i]}`);
	}
}

console.log(`A = [${arrayA.join(', ')}]`);
console.log(`B = [${arrayB.join(', ')}]`);
console.log(`C = [${arrayC.join(', ')}]`);

vectorAdd.destroy();
module.destroy();
bufferA.free();
bufferB.free();
bufferC.free();
context.destroy();
device.destroy();
