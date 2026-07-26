import { strict as assert } from 'node:assert';
import { describe, it } from 'node:test';

const isNativeRuntimeError = (error: unknown): boolean => {
	if (!(error instanceof Error) || !('code' in error)) {
		return false;
	}

	if (error.code === 'ERR_DLOPEN_FAILED') {
		return true;
	}

	return (
		error.code === 'MODULE_NOT_FOUND' &&
		(error.message.includes('cuda.node') || error.message.includes('segfault.node'))
	);
};

describe('CUDA Exported API', () => {
	it('loads the native API when the CUDA runtime is available', async (t) => {
		try {
			const cuda = await import('./index.ts');

			assert.strictEqual(typeof cuda.Ctx, 'function');
			assert.strictEqual(typeof cuda.Device, 'function');
			assert.strictEqual(typeof cuda.memAlloc, 'function');
			assert.strictEqual(typeof cuda.moduleRuntimeCompile, 'function');
			assert.strictEqual(typeof cuda.prepareArguments, 'function');
			assert.ok(cuda.cutilMathHeaderPath.endsWith('/include/util-math.h'));
			assert.ok(cuda.cutilMathInclude.includes('#include <'));
		} catch (error) {
			if (isNativeRuntimeError(error)) {
				t.skip('CUDA native runtime is unavailable');
				return;
			}
			throw error;
		}
	});
});
