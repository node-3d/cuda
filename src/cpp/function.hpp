#pragma once

#include <cuda.h>

#include "bindings.hpp"
#include "modulex.hpp"


class Function {
DECLARE_ES5_CLASS(Function, Function);
	
public:
	static void init(Napi::Env env, Napi::Object exports);
	explicit Function(const Napi::CallbackInfo& info);
	~Function();
	
	void _destroy();
	
	static Napi::Function getCtor() { return _ctorEs5.Value(); }
	
private:
	bool _isDestroyed;
	
	JS_DECLARE_GETTER(Function, isDestroyed);
	
	JS_DECLARE_METHOD(Function, destroy);
	JS_DECLARE_METHOD(Function, launchKernel);
	
	CUfunction m_function;
	
};
