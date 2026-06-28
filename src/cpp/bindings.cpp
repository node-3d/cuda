#include "bindings.hpp"

#include "ctx.hpp"
#include "device.hpp"
#include "function.hpp"
#include "mem.hpp"
#include "modulex.hpp"


#define JS_CUDA_SET_GETTER(name)                                                  \
	exports.DefineProperty(                                                       \
		Napi::PropertyDescriptor::Accessor(env, exports, #name, name)             \
	);


JS_METHOD(driverVersion) { NAPI_ENV;
	int version = 0;
	cuDriverGetVersion(&version);
	RET_NUM(version);
}

JS_METHOD(deviceCount) { NAPI_ENV;
	int count = 0;
	cuDeviceGetCount(&count);
	RET_NUM(count);
}


JS_METHOD(glDeviceCount) { NAPI_ENV;
	
	
	uint32_t deviceCount = 0;
	int32_t devices[8];
	
	cudaGLGetDevices(&deviceCount, devices, 8, cudaGLDeviceListAll);
	
	// flush NoDevice error if 0
	
	cudaGetLastError();
	
	
	// set device[0] - hmm
	
	//if (cudaGLSetGLDevice(devices[0]) != cudaSuccess)
	//assert(0);
	
	
	RET_NUM(deviceCount);
}


Napi::Object init(Napi::Env env, Napi::Object exports) {
	
	cuInit(0);
	
	JS_CUDA_SET_GETTER(driverVersion);
	JS_CUDA_SET_GETTER(deviceCount);
	JS_CUDA_SET_GETTER(glDeviceCount);
	
	
	
	Ctx::init(env, exports);
	Device::init(env, exports);
	Function::init(env, exports);
	Mem::init(env, exports);
	Modulex::init(env, exports);
	
	return exports;
	
}

NODE_API_MODULE(cuda, init)
