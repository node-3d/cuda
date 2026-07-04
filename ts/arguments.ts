import type { TCudaFunction, TDim3 } from './native.ts';

export type TKernelValueType =
	| 'Uint8'
	| 'Uint16'
	| 'Uint32'
	| 'Int8'
	| 'Int16'
	| 'Int32'
	| 'Float32'
	| 'Float64'
	| 'DevicePtr';

export type TKernelArgument = Readonly<{
	type: TKernelValueType | readonly TKernelValueType[];
	value: number | readonly number[];
}>;

const typeByteSize: Readonly<Record<TKernelValueType, number>> = {
	Uint8: 1,
	Uint16: 2,
	Uint32: 4,
	Int8: 1,
	Int16: 2,
	Int32: 4,
	Float32: 4,
	Float64: 8,
	DevicePtr: 8,
};

const typeAlignment: Readonly<Record<TKernelValueType, number>> = typeByteSize;

const alignUp = (offset: number, alignment: number): number =>
	(offset + alignment - 1) & ~(alignment - 1);

const isKernelValueTypeArray = (
	value: TKernelValueType | readonly TKernelValueType[],
): value is readonly TKernelValueType[] => Array.isArray(value);

const isNumberArray = (value: number | readonly number[]): value is readonly number[] =>
	Array.isArray(value);

const getArgumentTypes = (argument: TKernelArgument): readonly TKernelValueType[] =>
	isKernelValueTypeArray(argument.type) ? argument.type : [argument.type];

const getArgumentValue = (argument: TKernelArgument, index: number): number => {
	if (isNumberArray(argument.value)) {
		return argument.value[index] ?? 0;
	}
	return argument.value;
};

const writeArgument = (
	buffer: Buffer,
	type: TKernelValueType,
	value: number,
	offset: number,
): void => {
	switch (type) {
		case 'Uint8': {
			buffer.writeUInt8(value, offset);
			break;
		}
		case 'Uint16': {
			buffer.writeUInt16LE(value, offset);
			break;
		}
		case 'Uint32': {
			buffer.writeUInt32LE(value, offset);
			break;
		}
		case 'Int8': {
			buffer.writeInt8(value, offset);
			break;
		}
		case 'Int16': {
			buffer.writeInt16LE(value, offset);
			break;
		}
		case 'Int32': {
			buffer.writeInt32LE(value, offset);
			break;
		}
		case 'Float32': {
			buffer.writeFloatLE(value, offset);
			break;
		}
		case 'Float64': {
			buffer.writeDoubleLE(value, offset);
			break;
		}
		case 'DevicePtr': {
			buffer.writeBigUInt64LE(BigInt(value), offset);
			break;
		}
		default: {
			throw new TypeError(`Unsupported CUDA kernel argument type: ${type}`);
		}
	}
};

export const prepareArguments = (
	args: readonly TKernelArgument[] | Readonly<Record<string, TKernelArgument>>,
): Buffer => {
	const entries = Array.isArray(args) ? args : Object.values(args);
	let paramBufferSize = 0;

	for (const argument of entries) {
		const types = getArgumentTypes(argument);
		paramBufferSize = alignUp(paramBufferSize, typeAlignment[types[0]] * types.length);

		for (const type of types) {
			paramBufferSize += typeByteSize[type];
		}
	}

	const paramBuffer = Buffer.alloc(paramBufferSize);
	let offset = 0;

	for (const argument of entries) {
		const types = getArgumentTypes(argument);
		offset = alignUp(offset, typeAlignment[types[0]] * types.length);

		for (const [index, type] of types.entries()) {
			writeArgument(paramBuffer, type, getArgumentValue(argument, index), offset);
			offset += typeByteSize[type];
		}
	}

	return paramBuffer;
};

export const launch = (
	func: TCudaFunction,
	gridDim: TDim3,
	blockDim: TDim3,
	args: readonly TKernelArgument[] | Readonly<Record<string, TKernelArgument>>,
): number => func.launchKernel(gridDim, blockDim, prepareArguments(args));
