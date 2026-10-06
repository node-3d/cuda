import { install } from '@node-3d/addon-tools';

const prefix = 'https://github.com/node-3d/cuda/releases/download';
const tag = '1.1.1';

await install(`${prefix}/${tag}`);
