#pragma once

#include <uv.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/wglext.h>

#include <cuda.h>

// #define WGL_NV_gpu_affinity
#include <cuda_gl_interop.h>
// #undef WGL_NV_gpu_affinity

#include "bindings.hpp"
#include "device.hpp"

class Ctx {
	DECLARE_ES5_CLASS(Ctx, Ctx);

  public:
	static void init(Napi::Env env, Napi::Object exports);
	explicit Ctx(const Napi::CallbackInfo &info);
	~Ctx();

	CUresult _destroy();

  private:
	bool _isDestroyed;

	JS_DECLARE_GETTER(Ctx, isDestroyed);
	JS_DECLARE_GETTER(Ctx, apiVersion);

	JS_DECLARE_METHOD(Ctx, destroy);
	JS_DECLARE_METHOD(Ctx, pushCurrent);
	JS_DECLARE_METHOD(Ctx, popCurrent);
	JS_DECLARE_METHOD(Ctx, setCurrent);
	JS_DECLARE_METHOD(Ctx, getCurrent);
	JS_DECLARE_METHOD(Ctx, synchronize);

	CUcontext m_context;
	CUdevice m_device;
	bool sync_in_progress;
	uv_work_t work_req_;

	Napi::ObjectReference _that;
	Napi::AsyncContext _asyncCtx;

	static void process(uv_work_t *work_req);
	static void after(uv_work_t *work_req, int status);
};
