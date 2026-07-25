# @node-3d/cuda

This is a part of [Node3D](https://github.com/node-3d) project.

[![NPM](https://badge.fury.io/js/@node-3d%2Fcuda.svg)](https://badge.fury.io/js/@node-3d%2Fcuda)
[![Lint](https://github.com/node-3d/cuda/actions/workflows/lint.yml/badge.svg)](https://github.com/node-3d/cuda/actions/workflows/lint.yml)
[![Test](https://github.com/node-3d/cuda/actions/workflows/test.yml/badge.svg)](https://github.com/node-3d/cuda/actions/workflows/test.yml)
[![Cpplint](https://github.com/node-3d/cuda/actions/workflows/cpplint.yml/badge.svg)](https://github.com/node-3d/cuda/actions/workflows/cpplint.yml)

CUDA bindings for Node.js.

```bash
npm install @node-3d/cuda
```



This package exposes the CUDA driver API pieces used by Node3D examples:

* `Device`, `Ctx`, `Modulex`, `Function`, and `Mem` native wrappers.
* Memory helpers such as `memAlloc`, `memAllocPitch`, and `memVBO`.
* Module loading and runtime compilation helpers.
* `launch` and `prepareArguments` for typed kernel argument setup.
* Selected Thrust helpers used by compute-heavy examples.

```ts
import { Ctx, Device, getDeviceCount, memAlloc } from '@node-3d/cuda';

console.log('CUDA devices:', getDeviceCount());

const device = new Device(0);
const context = new Ctx(0, device);
const memory = memAlloc(1024);

memory.free();
context.destroy();
```

See `examples/basic.ts`, `examples/memory-copy.ts`, and `examples/vector-add.ts`
for complete runnable examples. CUDA toolkit/runtime availability is still required
on the host system.

## Binary Origin

Release archives are built by this repository's public GitHub Actions workflows.

Attestations: https://github.com/node-3d/cuda/attestations

To verify a downloaded archive:

```bash
gh release download <tag> -R node-3d/cuda -p <platform>.gz
gh attestation verify <platform>.gz -R node-3d/cuda
```
