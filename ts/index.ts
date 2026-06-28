import { fileURLToPath } from 'node:url';
import { native } from './native.ts';

export { launch, prepareArguments } from './arguments.ts';
export type { TKernelArgument, TKernelValueType } from './arguments.ts';

export type {
	TClockrateMultiprocessorCount,
	TComputeCapability,
	TCudaCtx,
	TCudaCtxConstructor,
	TCudaDevice,
	TCudaDeviceConstructor,
	TCudaFunction,
	TCudaFunctionConstructor,
	TCudaMem,
	TCudaMemConstructor,
	TCudaModule,
	TCudaModuleConstructor,
	TDim3,
} from './native.ts';

export const Ctx = native.Ctx;
export const Device = native.Device;
export const Function = native.Function;
export const Mem = native.Mem;
export const Modulex = native.Modulex;
export const memVBO = native.memVBO;
export const memAlloc = native.memAlloc;
export const memAllocPitch = native.memAllocPitch;
export const thrust_inclusiveScan = native.thrust_inclusiveScan;
export const thrust_reduce_floatSum = native.thrust_reduce_floatSum;
export const thrust_reduce_floatMax = native.thrust_reduce_floatMax;
export const thrust_reduce_floatMin = native.thrust_reduce_floatMin;
export const thrust_floatSort_int = native.thrust_floatSort_int;
export const thrust_remove_int = native.thrust_remove_int;
export const moduleLoad = native.moduleLoad;
export const moduleLoadData = native.moduleLoadData;
export const moduleRuntimeCompile = native.moduleRuntimeCompile;

export const getDriverVersion = (): number => native.driverVersion;
export const getDeviceCount = (): number => native.deviceCount;
export const getGlDeviceCount = (): number => native.glDeviceCount;

export const cutilMathHeaderUrl = new URL('../include/util-math.h', import.meta.url);
export const cutilMathHeaderPath = fileURLToPath(cutilMathHeaderUrl).replaceAll('\\', '/');
export const cutilMathInclude = `#include <${cutilMathHeaderPath}>\n`;
