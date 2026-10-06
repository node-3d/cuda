import assert from 'node:assert/strict';
import { existsSync } from 'node:fs';
import test from 'node:test';
import { Ctx, Device, cutilMathHeaderPath, prepareArguments } from '@node-3d/cuda';

test('loads the packed CUDA entry point', () => {
	assert.equal(typeof Ctx, 'function');
	assert.equal(typeof Device, 'function');
	assert.equal(typeof prepareArguments, 'function');
	assert.ok(existsSync(cutilMathHeaderPath));
});
