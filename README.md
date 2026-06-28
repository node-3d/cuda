# @node-3d/cuda

CUDA bindings for Node.js.

```console
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
