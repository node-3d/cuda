import { spawnSync } from 'node:child_process';
import { dirname, resolve } from 'node:path';

const nvccCommand = process.platform === 'win32' ? 'where' : 'which';
// oxlint-disable-next-line node/no-process-env
const envPath = process.env.CUDA_PATH || process.env.CUDA_HOME;

const findNvcc = () => {
	// oxlint-disable-next-line node/no-sync
	const result = spawnSync(nvccCommand, ['nvcc'], { encoding: 'utf8' });

	if (result.status !== 0) {
		throw new Error(`Unable to find nvcc with ${nvccCommand}`);
	}

	return result.stdout.split(/\r?\n/u)[0];
};

const nvccPath = envPath ? null : findNvcc();

// oxlint-disable-next-line no-console
console.log((envPath || resolve(dirname(nvccPath), '..')).replaceAll('\\', '/'));
