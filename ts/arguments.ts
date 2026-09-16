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
	valueType: TKernelValueType,
	value: number,
	offset: number,
): void => {
	switch (valueType) {
		case 'Uint8':
			buffer.writeUInt8(value, offset);
			break;
		case 'Uint16':
			buffer.writeUInt16LE(value, offset);
			break;
		case 'Uint32':
			buffer.writeUInt32LE(value, offset);
			break;
		case 'Int8':
			buffer.writeInt8(value, offset);
			break;
		case 'Int16':
			buffer.writeInt16LE(value, offset);
			break;
		case 'Int32':
			buffer.writeInt32LE(value, offset);
			break;
		case 'Float32':
			buffer.writeFloatLE(value, offset);
			break;
		case 'Float64':
			buffer.writeDoubleLE(value, offset);
			break;
		case 'DevicePtr':
			buffer.writeBigUInt64LE(BigInt(value), offset);
			break;
		default:
			valueType satisfies never;
			throw new TypeError(
				`Unsupported CUDA kernel argument valueType: ${valueType as string}`,
			);
	}
};

export const prepareArguments = (args: readonly TKernelArgument[]): Buffer => {
	let paramBufferSize = 0;

	for (const argument of args) {
		const types = getArgumentTypes(argument);
		const valType = types[0];
		if (!valType) {
			continue;
		}
		paramBufferSize = alignUp(paramBufferSize, typeAlignment[valType] * types.length);

		for (const type of types) {
			paramBufferSize += typeByteSize[type];
		}
	}

	const paramBuffer = Buffer.alloc(paramBufferSize);
	let offset = 0;

	for (const argument of args) {
		const types = getArgumentTypes(argument);
		const valType = types[0];
		if (!valType) {
			continue;
		}
		offset = alignUp(offset, typeAlignment[valType] * types.length);

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
	args: readonly TKernelArgument[],
): number => func.launchKernel(gridDim, blockDim, prepareArguments(args));
