import assert from 'node:assert/strict';
import test from 'node:test';
import { Ctx, Device, cutilMathHeaderPath, prepareArguments } from '@node-3d/cuda';

test('loads the packed CUDA entry point', () => {
	assert.equal(typeof Ctx, 'function');
	assert.equal(typeof Device, 'function');
	assert.equal(typeof prepareArguments, 'function');
	assert.match(cutilMathHeaderPath, /include\/util-math\.h$/u);
});
