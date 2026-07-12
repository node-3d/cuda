#include "function.hpp"
#include "mem.hpp"


IMPLEMENT_ES5_CLASS(Function);

void Function::init(Napi::Env env, Napi::Object exports) {
	Napi::Function ctor = wrap(env);

	JS_ASSIGN_GETTER(isDestroyed);

	JS_ASSIGN_METHOD(destroy);
	JS_ASSIGN_METHOD(launchKernel);

	exports.Set("Function", ctor);
}


Function::Function(const Napi::CallbackInfo &info) {
	super(info);

	_isDestroyed = false;
	m_function = nullptr;

	if (info.Length() == 1) {
		Napi::External<void> extFunc = info[0].As<Napi::External<void>>();
		m_function = reinterpret_cast<CUfunction>(extFunc.Data());
	}
}


Function::~Function() {
	// Function handles are invalidated only by explicit destroy() or their owning module.
	// GC finalization must not change native-visible lifetime.
}


void Function::_destroy() {
	DES_CHECK;
	m_function = nullptr;
	_isDestroyed = true;
}


JS_IMPLEMENT_METHOD(Function, destroy) {
	THIS_CHECK;
	_destroy();
	RET_UNDEFINED;
}


JS_IMPLEMENT_GETTER(Function, isDestroyed) {
	THIS_CHECK;
	RET_BOOL(_isDestroyed);
}

#include <iostream>
JS_IMPLEMENT_METHOD(Function, launchKernel) {
	THIS_CHECK;

	REQ_ARRAY_ARG(0, gridDim);
	REQ_ARRAY_ARG(1, blockDim);
	REQ_BUF_ARG(2, buf);

	uint32_t gridDimX = gridDim.Get(0U).ToNumber().Uint32Value();
	uint32_t gridDimY = gridDim.Get(1U).ToNumber().Uint32Value();
	uint32_t gridDimZ = gridDim.Get(2U).ToNumber().Uint32Value();
	uint32_t blockDimX = blockDim.Get(0U).ToNumber().Uint32Value();
	uint32_t blockDimY = blockDim.Get(1U).ToNumber().Uint32Value();
	uint32_t blockDimZ = blockDim.Get(2U).ToNumber().Uint32Value();

	uint8_t *pbuffer = buf.Data();
	size_t bufferSize = buf.Length();

	void *cuExtra[] = {
		CU_LAUNCH_PARAM_BUFFER_POINTER, pbuffer, CU_LAUNCH_PARAM_BUFFER_SIZE, &bufferSize, CU_LAUNCH_PARAM_END
	};

	CUresult error = cuLaunchKernel(
	    m_function, gridDimX, gridDimY, gridDimZ, blockDimX, blockDimY, blockDimZ, 0, 0, NULL, cuExtra
	);

	RET_NUM(error);
}
