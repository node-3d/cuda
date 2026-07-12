#include <functional>

#include "ctx.hpp"
#include "device.hpp"


IMPLEMENT_ES5_CLASS(Ctx);

void Ctx::init(Napi::Env env, Napi::Object exports) {
	Napi::Function ctor = wrap(env);

	JS_ASSIGN_GETTER(isDestroyed);
	JS_ASSIGN_GETTER(apiVersion);

	JS_ASSIGN_METHOD(destroy);
	JS_ASSIGN_METHOD(pushCurrent);
	JS_ASSIGN_METHOD(popCurrent);
	JS_ASSIGN_METHOD(setCurrent);
	JS_ASSIGN_METHOD(getCurrent);
	JS_ASSIGN_METHOD(synchronize);

	exports.Set("Ctx", ctor);
}


Ctx::Ctx(const Napi::CallbackInfo &info) : _asyncCtx(info.Env(), "Ctx") {
	super(info);

	_isDestroyed = false;
	sync_in_progress = false;

	unsigned int flags = info[0].ToNumber().Uint32Value();
	Napi::Object deviceObj = info[1].As<Napi::Object>();
	Device *device = Device::unwrap(deviceObj);

	_that.Reset(info.This().As<Napi::Object>(), 0);

#if CUDA_VERSION >= 13000
	cuCtxCreate(&m_context, nullptr, flags, device->getDevice());
#else
	cuCtxCreate(&m_context, flags, device->getDevice());
#endif
}


Ctx::~Ctx() {
	// Native CUDA contexts are released only by explicit destroy().
	// GC finalization must not invalidate resources still referenced elsewhere.
}


CUresult Ctx::_destroy() {
	if (_isDestroyed) {
		return CUDA_SUCCESS;
	}

	CUresult error = cuCtxDestroy(m_context);
	// do not: _that.Reset();
	_isDestroyed = true;
	return error;
}


JS_IMPLEMENT_GETTER(Ctx, apiVersion) {
	THIS_CHECK;
	unsigned int version;
	cuCtxGetApiVersion(m_context, &version);
	RET_NUM(version);
}


JS_IMPLEMENT_METHOD(Ctx, destroy) {
	THIS_CHECK;
	CUresult res = _destroy();
	RET_NUM(res);
}


JS_IMPLEMENT_GETTER(Ctx, isDestroyed) {
	THIS_CHECK;
	RET_BOOL(_isDestroyed);
}


JS_IMPLEMENT_METHOD(Ctx, pushCurrent) {
	THIS_CHECK;
	CUresult error = cuCtxPushCurrent(m_context);
	RET_NUM(error);
}

JS_IMPLEMENT_METHOD(Ctx, popCurrent) {
	THIS_CHECK;
	CUresult error = cuCtxPopCurrent(&m_context);
	RET_NUM(error);
}

JS_IMPLEMENT_METHOD(Ctx, setCurrent) {
	THIS_CHECK;
	CUresult error = cuCtxSetCurrent(m_context);
	RET_NUM(error);
}

JS_IMPLEMENT_METHOD(Ctx, getCurrent) {
	THIS_CHECK;
	CUresult error = cuCtxGetCurrent(&m_context);
	RET_NUM(error);
}


struct SynchronizeParams {
	Ctx *ctx;
	CUresult error;
	Napi::ThreadSafeFunction cb;
};

JS_IMPLEMENT_METHOD(Ctx, synchronize) {
	THIS_CHECK;

	// Async
	if (info.Length() >= 1 && info[0].IsFunction()) {
		if (sync_in_progress) {
			RET_NUM(-1);
		};

		REQ_FUN_ARG(0, cb);

		SynchronizeParams *params = new SynchronizeParams();
		params->ctx = this;
		params->cb = Napi::ThreadSafeFunction::New(env, info[0].As<Napi::Function>(), "Ctx", 0, 1);

		cuCtxPopCurrent(nullptr);

		// build up the work request
		uv_work_t *work_req = new uv_work_t;
		work_req->data = params;

		uv_queue_work(uv_default_loop(), work_req, process, after);

		_that.Ref();
		sync_in_progress = true;

		RET_UNDEFINED;

	} else {
		// Synchronous
		CUresult error = cuCtxSynchronize();
		RET_NUM(error);
	}
}


void Ctx::process(uv_work_t *work_req) {
	SynchronizeParams *params = static_cast<SynchronizeParams *>(work_req->data);

	params->error = cuCtxPushCurrent(params->ctx->m_context);
	if (params->error) {
		return;
	}

	params->error = cuCtxSynchronize();
	if (params->error) {
		return;
	}

	params->error = cuCtxPopCurrent(nullptr);
}


void Ctx::after(uv_work_t *work_req, int status) {
	SynchronizeParams *params = static_cast<SynchronizeParams *>(work_req->data);
	Ctx *ctx = params->ctx;
	ctx->sync_in_progress = false;

	cuCtxPushCurrent(ctx->m_context);

	Napi::Env env = ctx->_that.Env();
	NAPI_HS;

	auto callback = [](Napi::Env env, Napi::Function jsCallback, CUresult *value) {
		jsCallback.Call({ JS_NUM(*value) });
		delete value;
	};

	CUresult *value = new CUresult(params->error);
	napi_status napiStatus = params->cb.BlockingCall(value, callback);

	if (napiStatus != napi_ok) {
		std::cerr << "Error: could not call the threadsafe function." << std::endl;
	}

	params->cb.Release();

	delete params;
	delete work_req;
	ctx->_that.Unref();
}
