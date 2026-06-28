import { Ctx, Device, getDeviceCount, getDriverVersion, memAlloc } from '@node-3d/cuda';

const deviceCount = getDeviceCount();

console.log('CUDA driver version:', getDriverVersion());
console.log('CUDA devices:', deviceCount);

if (deviceCount === 0) {
	throw new Error('No CUDA devices available');
}

for (let deviceId = 0; deviceId < deviceCount; deviceId++) {
	const device = new Device(deviceId);
	console.log(`Device ${deviceId}: ${device.name}`);
	console.log('  total memory:', device.totalMem);
	console.log(
		'  compute capability:',
		`${device.computeCapability.major}.${device.computeCapability.minor}`,
	);
	device.destroy();
}

const device = new Device(0);
const context = new Ctx(0, device);
const memory = memAlloc(100);

console.log('Allocated bytes:', memory.size);

memory.free();
context.destroy();
device.destroy();
