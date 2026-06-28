#pragma once

#include <cuda.h>
#include <cuda_runtime_api.h>

#include "bindings.hpp"


class Device {
DECLARE_ES5_CLASS(Device, Device);
	
public:
	static void init(Napi::Env env, Napi::Object exports);
	explicit Device(const Napi::CallbackInfo& info);
	~Device();
	
	void _destroy();
	
	CUdevice getDevice() const { return m_device; }
	
private:
	bool _isDestroyed;
	
	JS_DECLARE_GETTER(Device, isDestroyed);
	JS_DECLARE_GETTER(Device, computeCapability);
	JS_DECLARE_GETTER(Device, name);
	JS_DECLARE_GETTER(Device, totalMem);
	
	JS_DECLARE_GETTER(Device, clockrateMultiprocessorCount);
	
	JS_DECLARE_METHOD(Device, destroy);
	JS_DECLARE_METHOD(Device, limitMallocHeapSize);
	
	CUdevice m_device;
	
};
