import { Ctx, Device, getDeviceCount, memAlloc } from '@node-3d/cuda';

if (getDeviceCount() === 0) {
	throw new Error('No CUDA devices available');
}

const source = new Uint32Array([1, 2, 3, 4, 5]);
const destination = new Uint32Array(source.length);

const device = new Device(0);
const context = new Ctx(0, device);
const memory = memAlloc(source.byteLength);

memory.copyHtoD(source);
memory.copyDtoH(destination);

console.log(`source = [${source.join(', ')}]`);
console.log(`copy = [${destination.join(', ')}]`);

memory.free();
context.destroy();
device.destroy();
