import { strict as assert } from 'node:assert';
import { describe, it } from 'node:test';
import { prepareArguments } from './arguments.ts';

describe('CUDA Kernel Arguments', () => {
	it('packs scalar values with CUDA launch alignment', () => {
		const buffer = prepareArguments([
			{ type: 'Uint8', value: 0x12 },
			{ type: 'Uint32', value: 0x12345678 },
			{ type: 'Float32', value: 1.5 },
		]);

		assert.strictEqual(buffer.length, 12);
		assert.strictEqual(buffer.readUInt8(0), 0x12);
		assert.strictEqual(buffer.readUInt32LE(4), 0x12345678);
		assert.strictEqual(buffer.readFloatLE(8), 1.5);
	});

	it('packs vector values as a single aligned argument', () => {
		const buffer = prepareArguments([
			{ type: ['Float32', 'Float32', 'Float32'], value: [1, 2, 3] },
			{ type: 'Int32', value: 4 },
		]);

		assert.strictEqual(buffer.length, 16);
		assert.deepStrictEqual([
			buffer.readFloatLE(0),
			buffer.readFloatLE(4),
			buffer.readFloatLE(8),
			buffer.readInt32LE(12),
		], [1, 2, 3, 4]);
	});

	it('packs device pointers as 64-bit values', () => {
		const buffer = prepareArguments([
			{ type: 'DevicePtr', value: 0x12345678 },
		]);

		assert.strictEqual(buffer.length, 8);
		assert.strictEqual(buffer.readBigUInt64LE(0), 0x12345678n);
	});
});
