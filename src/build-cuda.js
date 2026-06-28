import { existsSync, mkdirSync } from 'node:fs';
import { dirname, basename, isAbsolute, resolve } from 'node:path';
import { spawnSync } from 'node:child_process';

const scriptDir = import.meta.dirname;
const isWindows = process.platform === 'win32';
const [inputRaw, outputRaw] = process.argv.slice(2);

if (!inputRaw || !outputRaw) {
	throw new Error('Usage: node build-cuda.js <input.cu> <output.obj>');
}

const cleanPath = (value) => value.replaceAll('"', '');

const normalizeInput = (value) => {
	let current = cleanPath(value).replaceAll('\\', '/');
	let resolved = resolve(current);

	while (!existsSync(resolved) && current.startsWith('../')) {
		current = current.slice(3);
		resolved = resolve(current);
	}

	if (!existsSync(resolved)) {
		resolved = resolve(scriptDir, 'cpp', basename(current));
	}

	return resolved;
};

const normalizeOutput = (value) => {
	let current = cleanPath(value).replaceAll('\\', '/');

	if (isAbsolute(current)) {
		return current;
	}

	while (current.startsWith('../')) {
		current = current.slice(3);
	}

	if (current.startsWith('build/')) {
		current = current.slice(6);
	}

	return resolve(scriptDir, 'build', current);
};

const input = normalizeInput(inputRaw);
const output = normalizeOutput(outputRaw);
const compilerOptions = isWindows
	? ['/MD', '/Zc:preprocessor', '/D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH']
	: ['-fPIC'];

mkdirSync(dirname(output), { recursive: true });

const args = [
	'--std=c++17',
	'-use_fast_math',
	'-prec-div=false',
	'-prec-sqrt=false',
	'--machine=64',
	'--gpu-architecture=compute_75',
	'--allow-unsupported-compiler',
	`--compiler-options=${compilerOptions.join(',')}`,
	'-c',
	input,
	'-o',
	output,
];

const result = spawnSync('nvcc', args, {
	stdio: 'inherit',
});

process.exit(result.status ?? 1);
