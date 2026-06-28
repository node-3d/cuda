#include "device.hpp"


IMPLEMENT_ES5_CLASS(Device);

void Device::init(Napi::Env env, Napi::Object exports) {
	
	Napi::Function ctor = wrap(env);
	
	JS_ASSIGN_GETTER(isDestroyed);
	JS_ASSIGN_GETTER(name);
	JS_ASSIGN_GETTER(totalMem);
	JS_ASSIGN_GETTER(computeCapability);
	JS_ASSIGN_GETTER(clockrateMultiprocessorCount);
	
	JS_ASSIGN_METHOD(destroy);
	JS_ASSIGN_METHOD(limitMallocHeapSize);
	
	exports.Set("Device", ctor);
	
}


Device::Device(const Napi::CallbackInfo &info) {
	
	super(info);
	
	_isDestroyed = false;
	
	int32_t id = info[0].ToNumber().Int32Value();
	cuDeviceGet(&m_device, id);
	
}


Device::~Device() {
	// Device is a lightweight handle; explicit destroy() only marks this wrapper destroyed.
}


void Device::_destroy() { DES_CHECK;
	
	_isDestroyed = true;
	
}


JS_IMPLEMENT_GETTER(Device, name) { THIS_CHECK;
	char deviceName[256];
	cuDeviceGetName(deviceName, 256, m_device);
	RET_STR(deviceName);
}

JS_IMPLEMENT_GETTER(Device, totalMem) { THIS_CHECK;
	size_t totalGlobalMem;
	cuDeviceTotalMem(&totalGlobalMem, m_device);
	RET_NUM(totalGlobalMem);
}

JS_IMPLEMENT_GETTER(Device, computeCapability) { THIS_CHECK;
	int major = 0;
	int minor = 0;
	cuDeviceGetAttribute(&major, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR, m_device);
	cuDeviceGetAttribute(&minor, CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MINOR, m_device);
	Napi::Object obj = Napi::Object::New(env);
	obj.Set("major", JS_NUM(major));
	obj.Set("minor", JS_NUM(minor));
	RET_VALUE(obj);
}

JS_IMPLEMENT_GETTER(Device, clockrateMultiprocessorCount) { THIS_CHECK;
	int major = 0;
	int minor = 0;
	cuDeviceGetAttribute(&major, CU_DEVICE_ATTRIBUTE_CLOCK_RATE, m_device);
	cuDeviceGetAttribute(&minor, CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, m_device);
	Napi::Object obj = Napi::Object::New(env);
	obj.Set("clockrate", JS_NUM(major));
	obj.Set("multiprocessor_count", JS_NUM(minor));
	RET_VALUE(obj);
}

JS_IMPLEMENT_METHOD(Device, destroy) { THIS_CHECK;
	_destroy();
	RET_UNDEFINED;
}


JS_IMPLEMENT_GETTER(Device, isDestroyed) { THIS_CHECK;
	RET_BOOL(_isDestroyed);
}


JS_IMPLEMENT_METHOD(Device, limitMallocHeapSize) { THIS_CHECK;
	REQ_OFFS_ARG(0, bytesize);
	cudaDeviceSetLimit(cudaLimitMallocHeapSize, bytesize);
	RET_UNDEFINED;
}
