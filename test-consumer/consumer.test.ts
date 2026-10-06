import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { existsSync } from 'node:fs';
import { createRequire } from 'node:module';
import path from 'node:path';
import test from 'node:test';

const require = createRequire(import.meta.url);
const packageRoot = path.resolve(path.dirname(require.resolve('@node-3d/cuda')), '..');

const binaryDirectory = (): string => {
	const target = `${process.platform}-${process.arch}`;
	const names: Record<string, string> = {
		'win32-x64': 'windows',
		'linux-x64': 'linux',
		'linux-arm64': 'aarch64',
	};
	return `bin-${names[target] ?? target}`;
};

const hasCudaDriver = (): boolean => {
	if (process.platform === 'win32') {
		// oxlint-disable-next-line node/no-process-env
		const systemRoot = process.env.SystemRoot ?? String.raw`C:\Windows`;
		return existsSync(path.join(systemRoot, 'System32', 'nvcuda.dll'));
	}
	const result = spawnSync('ldconfig', ['-p'], { encoding: 'utf8' });
	return result.status === 0 && result.stdout.includes('libcuda.so.1');
};

test('installs the packed CUDA binary and header', () => {
	assert.ok(existsSync(path.join(packageRoot, 'include', 'util-math.h')));
	if (process.platform !== 'darwin') {
		const binaryPath = path.join(packageRoot, binaryDirectory(), 'cuda.node');
		assert.ok(existsSync(binaryPath));
	}
});

test('loads the public CUDA entry when the driver is available', async (t) => {
	if (process.platform !== 'darwin' && !hasCudaDriver()) {
		t.skip('NVIDIA CUDA driver is unavailable on this runner');
		return;
	}
	const cuda = await import('@node-3d/cuda');
	assert.equal(typeof cuda.Ctx, 'function');
	assert.equal(typeof cuda.Device, 'function');
	assert.equal(typeof cuda.prepareArguments, 'function');
	assert.ok(existsSync(cuda.cutilMathHeaderPath));
});
