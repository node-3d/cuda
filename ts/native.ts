import { createRequire } from 'node:module';
import { getBin } from '@node-3d/addon-tools';
import '@node-3d/segfault';

export type TDim3 = readonly [x: number, y: number, z: number];

export type TComputeCapability = Readonly<{
	major: number;
	minor: number;
}>;

export type TClockrateMultiprocessorCount = Readonly<{
	clockrate: number;
	multiprocessor_count: number;
}>;

export type TCudaDevice = Readonly<{
	isDestroyed: boolean;
	name: string;
	totalMem: number;
	computeCapability: TComputeCapability;
	clockrateMultiprocessorCount: TClockrateMultiprocessorCount;
	destroy: () => void;
	limitMallocHeapSize: (bytesize: number) => void;
}>;

export type TCudaDeviceConstructor = {
	new(deviceId: number): TCudaDevice;
	readonly prototype: TCudaDevice;
};

export type TCudaCtx = Readonly<{
	isDestroyed: boolean;
	apiVersion: number;
	destroy: () => number;
	pushCurrent: () => number;
	popCurrent: () => number;
	setCurrent: () => number;
	getCurrent: () => number;
	synchronize: {
		(): number;
		(cb: (error: number) => void): void;
	};
}>;

export type TCudaCtxConstructor = {
	new(flags: number, device: TCudaDevice): TCudaCtx;
	readonly prototype: TCudaCtx;
};

export type TCudaFunction = Readonly<{
	isDestroyed: boolean;
	name?: string;
	error?: number;
	destroy: () => void;
	launchKernel: (gridDim: TDim3, blockDim: TDim3, args: Buffer) => number;
}>;

export type TCudaFunctionConstructor = {
	readonly prototype: TCudaFunction;
};

export type TCudaMem = Readonly<{
	isDestroyed: boolean;
	devicePtr: number;
	size?: number;
	pitch?: number;
	error?: number;
	destroy: () => void;
	free: () => number;
	copyHtoD: (host: NodeJS.ArrayBufferView, bytes?: number | null, async?: boolean) => number;
	copyDtoH: (host: NodeJS.ArrayBufferView, bytes?: number | null, async?: boolean) => number;
	copyDtoH_ByPtr: (
		devicePtr: number,
		host: NodeJS.ArrayBufferView,
		bytes?: number | null,
		async?: boolean
	) => number;
	copyHtoD_A: (...args: readonly unknown[]) => unknown;
	copyDtoH_A: (...args: readonly unknown[]) => unknown;
	cudaMemset: (value: number, bytes?: number | null) => number;
	initVBO: (vboId: number) => void;
	regVBO: (vboId: number) => void;
	unregVBO: () => void;
}>;

export type TCudaMemConstructor = {
	readonly prototype: TCudaMem;
};

export type TCudaModule = Readonly<{
	isDestroyed: boolean;
	fname?: string;
	error?: number;
	log?: string | null;
	destroy: () => void;
	getFunction: (name: string) => TCudaFunction;
}>;

export type TCudaModuleConstructor = {
	readonly prototype: TCudaModule;
};

type TNative = Readonly<{
	driverVersion: number;
	deviceCount: number;
	glDeviceCount: number;
	Ctx: TCudaCtxConstructor;
	Device: TCudaDeviceConstructor;
	Function: TCudaFunctionConstructor;
	Mem: TCudaMemConstructor;
	Modulex: TCudaModuleConstructor;
	memVBO: () => TCudaMem;
	memAlloc: (bytesize: number) => TCudaMem;
	memAllocPitch: (elements: number, height: number, elementSizeBytes: number) => TCudaMem;
	thrust_inclusiveScan: (source: TCudaMem, count: number, destination: TCudaMem) => void;
	thrust_reduce_floatSum: (source: TCudaMem, count: number) => number;
	thrust_reduce_floatMax: (source: TCudaMem, count: number) => [position: number, value: number];
	thrust_reduce_floatMin: (source: TCudaMem, count: number) => [position: number, value: number];
	thrust_floatSort_int: (keys: TCudaMem, count: number, values: TCudaMem) => void;
	thrust_remove_int: (source: TCudaMem, count: number, value: number) => number;
	moduleLoad: (fileName: string) => TCudaModule;
	moduleLoadData: (fileName: string, ptx: string) => TCudaModule;
	moduleRuntimeCompile: (fileName: string, source: string, outputFile?: string) => TCudaModule;
}>;

const loadAddon = createRequire(import.meta.url);

export const native = loadAddon(`../${getBin()}/cuda.node`) as TNative;
