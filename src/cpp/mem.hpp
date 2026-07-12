#pragma once

#include <cuda.h>

#include <GL/glew.h>

#include <GL/wglext.h>

#include <cuda_gl_interop.h>
#include <cuda_runtime_api.h>

#include "bindings.hpp"
#include "function.hpp"


class Mem {
	DECLARE_ES5_CLASS(Mem, Mem);

  public:
	static void init(Napi::Env env, Napi::Object exports);
	explicit Mem(const Napi::CallbackInfo &info);
	~Mem();

	void _destroy();

  private:
	bool _isDestroyed;

	JS_DECLARE_GETTER(Mem, isDestroyed);
	JS_DECLARE_GETTER(Mem, devicePtr);

	JS_DECLARE_METHOD(Mem, destroy);

	static JS_METHOD(memVBO);
	static JS_METHOD(memAlloc);
	static JS_METHOD(memAllocPitch);
	static JS_METHOD(thrust_inclusiveScan);
	static JS_METHOD(thrust_reduce_floatSum);
	static JS_METHOD(thrust_reduce_floatMax);
	static JS_METHOD(thrust_reduce_floatMin);
	static JS_METHOD(thrust_floatSort_int);
	static JS_METHOD(thrust_remove_int);

	JS_DECLARE_METHOD(Mem, free);
	JS_DECLARE_METHOD(Mem, copyHtoD);
	JS_DECLARE_METHOD(Mem, copyDtoH);
	JS_DECLARE_METHOD(Mem, copyDtoH_ByPtr);
	JS_DECLARE_METHOD(Mem, copyHtoD_A);
	JS_DECLARE_METHOD(Mem, copyDtoH_A);
	JS_DECLARE_METHOD(Mem, cudaMemset);
	JS_DECLARE_METHOD(Mem, initVBO);
	JS_DECLARE_METHOD(Mem, regVBO);
	JS_DECLARE_METHOD(Mem, unregVBO);

	CUdeviceptr m_devicePtr;
	struct cudaGraphicsResource *vbo;
	size_t _totalBytes;
};
