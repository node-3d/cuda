# @node-3d/cuda

This is a part of [Node3D](https://github.com/node-3d) project.

[![NPM](https://badge.fury.io/js/@node-3d%2Fcuda.svg)](https://badge.fury.io/js/@node-3d/cuda)
[![Lint](https://github.com/node-3d/cuda/actions/workflows/lint.yml/badge.svg)](https://github.com/node-3d/cuda/actions/workflows/lint.yml)
[![Test](https://github.com/node-3d/cuda/actions/workflows/test.yml/badge.svg)](https://github.com/node-3d/cuda/actions/workflows/test.yml)
[![Cpplint](https://github.com/node-3d/cuda/actions/workflows/cpplint.yml/badge.svg)](https://github.com/node-3d/cuda/actions/workflows/cpplint.yml)

CUDA bindings for Node.js.

```console
npm install @node-3d/cuda
```

## Binary Provenance

Release binary archives for this package are published by public GitHub Actions
workflows.
When a release asset includes a GitHub Artifact Attestation, you can verify that
the downloaded archive was published by the visible workflow in this repository:

```console
gh release download <tag> -R node-3d/cuda -p <platform>.gz
gh attestation verify <platform>.gz -R node-3d/cuda
```

The attestation links the archive to the repository, workflow, commit, and
release build identity. It does not replace review of the source code, workflow,
or third-party library contents.

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
