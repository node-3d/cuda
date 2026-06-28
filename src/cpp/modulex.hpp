#pragma once

#include <nvrtc.h>
#include <cuda.h>

#include "bindings.hpp"


class Modulex {
DECLARE_ES5_CLASS(Modulex, Modulex);
	
public:
	static void init(Napi::Env env, Napi::Object exports);
	explicit Modulex(const Napi::CallbackInfo& info);
	~Modulex();
	
	void _destroy();
	
private:
	bool _isDestroyed;
	
	JS_DECLARE_GETTER(Modulex, isDestroyed);
	
	JS_DECLARE_METHOD(Modulex, destroy);
	static JS_METHOD(moduleLoad);
	static JS_METHOD(moduleLoadData);
	static JS_METHOD(moduleRuntimeCompile);
	JS_DECLARE_METHOD(Modulex, getFunction);
	
	CUmodule m_module;
	
};
