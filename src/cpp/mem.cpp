#include "mem.hpp"
#include "bindings.hpp"

#include "thrust_func.cuh"


// 2021-12-10 Pantelyuk fix


void cuda_vbo_init(GLuint vboId2, void **cuda, cudaGraphicsResource **cuda_vbo)
{
  cudaError_t err;
  GLuint vboId = vboId2;
  err = cudaGraphicsGLRegisterBuffer(cuda_vbo, vboId, cudaGraphicsMapFlagsNone);
};

void cuda_vbo_free(cudaGraphicsResource **cuda_vbo){
    cudaGraphicsUnregisterResource(*cuda_vbo);
};

void cuda_vbo_reg(GLuint vboId2, void **cuda, cudaGraphicsResource **cuda_vbo)
{
    cudaError_t err;
    GLuint vboId = vboId2;
    err = cudaGraphicsMapResources(1, cuda_vbo, 0);
    size_t num_bytes;
    err = cudaGraphicsResourceGetMappedPointer(cuda, &num_bytes, *cuda_vbo);
};

void cuda_vbo_unreg(cudaGraphicsResource **cuda_vbo){
    cudaGraphicsUnmapResources(1, cuda_vbo, 0);
};


/*
void cuda_vbo_reg(GLuint vboId2, void **cuda, cudaGraphicsResource **cuda_vbo)
{
		cudaError_t err;
		GLuint vboId = vboId2;

		err = cudaGraphicsGLRegisterBuffer(cuda_vbo, vboId, cudaGraphicsMapFlagsNone);

		err = cudaGraphicsMapResources(1, cuda_vbo, 0);
		size_t num_bytes;
		err = cudaGraphicsResourceGetMappedPointer(cuda, &num_bytes, *cuda_vbo);

};

void cuda_vbo_unreg(cudaGraphicsResource **cuda_vbo){
		cudaGraphicsUnmapResources(1, cuda_vbo, 0);
		cudaGraphicsUnregisterResource(*cuda_vbo);
};
*/

IMPLEMENT_ES5_CLASS(Mem);

void Mem::init(Napi::Env env, Napi::Object exports) {
	
	Napi::Function ctor = wrap(env);
	
	JS_ASSIGN_GETTER(isDestroyed);
	JS_ASSIGN_GETTER(devicePtr);
	JS_ASSIGN_METHOD(destroy);
	
	exports.DefineProperties({
		SF(memVBO),
		SF(memAlloc),
		SF(memAllocPitch),
		SF(thrust_inclusiveScan),
		SF(thrust_reduce_floatSum),
		SF(thrust_reduce_floatMax),
		SF(thrust_reduce_floatMin),
		SF(thrust_floatSort_int),
		SF(thrust_remove_int)
	});
	
	JS_ASSIGN_METHOD(free);
	JS_ASSIGN_METHOD(copyHtoD);
	JS_ASSIGN_METHOD(copyDtoH);
	JS_ASSIGN_METHOD(copyDtoH_ByPtr);
	JS_ASSIGN_METHOD(copyHtoD_A);
	JS_ASSIGN_METHOD(copyDtoH_A);
	JS_ASSIGN_METHOD(cudaMemset);
	JS_ASSIGN_METHOD(initVBO);
	JS_ASSIGN_METHOD(regVBO);
	JS_ASSIGN_METHOD(unregVBO);
	
	exports.Set("Mem", ctor);
	
}


Mem::Mem(const Napi::CallbackInfo &info) { NAPI_ENV;
	
	super(info);
	
	_isDestroyed = false;
	m_devicePtr = 0;
	vbo = nullptr;
	_totalBytes = 0;
	
	if (info.Length() == 1) {
		size_t bytesize = info[0].ToNumber().Uint32Value();
		CUresult error = cuMemAlloc(&m_devicePtr, bytesize);
		_totalBytes = bytesize;
		Napi::Object that = info.This().As<Napi::Object>();
		that.Set("size", JS_NUM(bytesize));
		that.Set("error", JS_NUM(error));
	} else if (info.Length() == 3) {
		size_t pPitch;
		size_t elements = info[0].ToNumber().Uint32Value();
		size_t height = info[1].ToNumber().Uint32Value();
		size_t elementSizeBytes = info[2].ToNumber().Uint32Value();
		size_t widthInBytes = elementSizeBytes * elements;
		CUresult error = cuMemAllocPitch(
			&m_devicePtr,
			&pPitch,
			widthInBytes,
			height,
			elementSizeBytes
		);
		_totalBytes = pPitch * height;
		Napi::Object that = info.This().As<Napi::Object>();
		that.Set("size", JS_NUM(pPitch * height));
		that.Set("pitch", JS_NUM(pPitch));
		that.Set("error", JS_NUM(error));
	}
	
}


Mem::~Mem() {
	// Device memory is released only by explicit free().
	// GC finalization must not invalidate pointers still referenced elsewhere.
}


void Mem::_destroy() { DES_CHECK;
	m_devicePtr = 0;
	vbo = nullptr;
	_totalBytes = 0;
	_isDestroyed = true;
}


JS_IMPLEMENT_GETTER(Mem, devicePtr) { THIS_CHECK;
	RET_NUM(static_cast<size_t>(m_devicePtr));
}


JS_IMPLEMENT_METHOD(Mem, destroy) { THIS_CHECK;
	_destroy();
	RET_UNDEFINED;
}


JS_IMPLEMENT_GETTER(Mem, isDestroyed) { THIS_CHECK;
	RET_BOOL(_isDestroyed);
}


JS_METHOD(Mem::memVBO) { NAPI_ENV;
	RET_VALUE(_ctorEs5.Value().New({}));
}


JS_METHOD(Mem::memAlloc) { NAPI_ENV;
	REQ_UINT32_ARG(0, bytesize);
	napi_value argv[1];
	argv[0] = JS_NUM(bytesize);
	RET_VALUE(_ctorEs5.Value().New(1, argv));
}


JS_METHOD(Mem::memAllocPitch) { NAPI_ENV;
	REQ_UINT32_ARG(0, elements);
	REQ_UINT32_ARG(1, height);
	REQ_UINT32_ARG(2, elementSizeBytes);
	napi_value argv[3];
	argv[0] = JS_NUM(elements);
	argv[1] = JS_NUM(height);
	argv[2] = JS_NUM(elementSizeBytes);
	RET_VALUE(_ctorEs5.Value().New(3, argv));
}


JS_METHOD(Mem::thrust_inclusiveScan) { NAPI_ENV;
	
	REQ_OBJ_ARG(0, memObj1);
	Mem *pmem1 = Mem::unwrap(memObj1);
	REQ_UINT32_ARG(1, cnt);
	REQ_OBJ_ARG(2, memObj2);
	Mem *pmem2 = Mem::unwrap(memObj2);
	
	uint32_t *ptr1 = (uint32_t *)(pmem1->m_devicePtr);
	uint32_t *ptr2 = (uint32_t *)(pmem2->m_devicePtr);
	
	thrust_inclusiveScan_(ptr1, ptr1 + cnt, ptr2);
	
	RET_UNDEFINED;
	
}


JS_METHOD(Mem::thrust_reduce_floatSum) { NAPI_ENV;
	
	REQ_OBJ_ARG(0, memObj);
	Mem *pmem1 = Mem::unwrap(memObj);
	REQ_UINT32_ARG(1, cnt);
	
	float *ptr1 = (float *)(pmem1->m_devicePtr);
	float result = thrust_reduce_floatSum_(ptr1, ptr1 + cnt);
	
	RET_NUM(result);
	
}


JS_METHOD(Mem::thrust_reduce_floatMax) { NAPI_ENV;
	
	REQ_OBJ_ARG(0, memObj);
	Mem *pmem1 = Mem::unwrap(memObj);
	REQ_UINT32_ARG(1, cnt);
	
	float *ptr1 = (float *)(pmem1->m_devicePtr);
	posval rv = thrust_reduce_floatMax_(ptr1, ptr1 + cnt);
	
	Napi::Array ra = Napi::Array::New(env);
	ra.Set(0U, rv.pos);
	ra.Set(1U, rv.val);
	RET_VALUE(ra);
	
}


JS_METHOD(Mem::thrust_reduce_floatMin) { NAPI_ENV;
	
	REQ_OBJ_ARG(0, memObj);
	Mem *pmem1 = Mem::unwrap(memObj);
	REQ_UINT32_ARG(1, cnt);
	
	float *ptr1 = (float *)(pmem1->m_devicePtr);
	posval rv = thrust_reduce_floatMin_(ptr1, ptr1 + cnt);
	
	Napi::Array ra = Napi::Array::New(env);
	ra.Set(0U, rv.pos);
	ra.Set(1U, rv.val);
	RET_VALUE(ra);
	
}


JS_METHOD(Mem::thrust_floatSort_int) { NAPI_ENV;
	
	REQ_OBJ_ARG(0, memObj1);
	Mem *pmem1 = Mem::unwrap(memObj1);
	REQ_UINT32_ARG(1, cnt);
	REQ_OBJ_ARG(2, memObj2);
	Mem *pmem2 = Mem::unwrap(memObj2);
	
	float *ptr1 = (float *)(pmem1->m_devicePtr);
	uint32_t *ptr2 = (uint32_t *)(pmem2->m_devicePtr);
		
	thrust_floatSort_int_(ptr1, ptr1 + cnt, ptr2);
	RET_UNDEFINED;
	
}


JS_METHOD(Mem::thrust_remove_int) { NAPI_ENV;
	
	REQ_OBJ_ARG(0, memObj1);
	Mem *pmem1 = Mem::unwrap(memObj1);
	REQ_UINT32_ARG(1, cnt);
	REQ_INT32_ARG(2, value);
	
	int32_t *ptr1 = (int32_t *)(pmem1->m_devicePtr);
	int32_t new_end = thrust_remove_int_(ptr1, ptr1 + cnt, value);
	
	RET_NUM(new_end);
	
}


JS_IMPLEMENT_METHOD(Mem, free) { THIS_CHECK;
	CUresult error = cuMemFree(m_devicePtr);
	if (error == CUDA_SUCCESS) {
		_destroy();
	}
	RET_NUM(error);
}


JS_IMPLEMENT_METHOD(Mem, copyHtoD) { THIS_CHECK;
	
	REQ_OBJ_ARG(0, buff32);
	USE_UINT32_ARG(1, bytes, _totalBytes);
	LET_BOOL_ARG(2, async);
	
	const uint8_t* phost = getArrayData(env, buff32);
	
	CUresult error;
	if (async) {
		error = cuMemcpyHtoDAsync(m_devicePtr, phost, bytes, 0);
	} else {
		error = cuMemcpyHtoD(m_devicePtr, phost, bytes);
	}
	
	RET_NUM(error);
	
}


JS_IMPLEMENT_METHOD(Mem, copyDtoH) { THIS_CHECK;
	
	REQ_OBJ_ARG(0, buff32);
	USE_UINT32_ARG(1, bytes, _totalBytes);
	LET_BOOL_ARG(2, async);
	
	uint8_t* phost = const_cast<uint8_t*>(getArrayData(env, buff32));
	
	CUresult error;
	if (async) {
		error = cuMemcpyDtoHAsync(phost, m_devicePtr, bytes, 0);
	} else {
		error = cuMemcpyDtoH(phost, m_devicePtr, bytes);
	}
	
	RET_NUM(error);
	
}


JS_IMPLEMENT_METHOD(Mem, copyDtoH_ByPtr) { THIS_CHECK;
	
	REQ_OFFS_ARG(0, devicePtr);
	REQ_OBJ_ARG(1, buff32);
	USE_UINT32_ARG(2, bytes, _totalBytes);
	LET_BOOL_ARG(3, async);
	
	uint8_t* phost = const_cast<uint8_t*>(getArrayData(env, buff32));
	
	CUresult error;
	if (async) {
		error = cuMemcpyDtoHAsync(phost, devicePtr, bytes, 0);
	} else {
		error = cuMemcpyDtoH(phost, devicePtr, bytes);
	}
	
	RET_NUM(error);
	
}


JS_IMPLEMENT_METHOD(Mem, copyHtoD_A) { THIS_CHECK;
	RET_VALUE(__i_copyHtoD(info));
}


JS_IMPLEMENT_METHOD(Mem, copyDtoH_A) { THIS_CHECK;
	RET_VALUE(__i_copyDtoH(info));
}


JS_IMPLEMENT_METHOD(Mem, cudaMemset) { THIS_CHECK;
	
	REQ_INT32_ARG(0, value);
	USE_UINT32_ARG(1, bytes, _totalBytes);
	
	cudaMemset(reinterpret_cast<void *>(m_devicePtr), value, bytes);
	
	RET_NUM(0);
	
}


JS_IMPLEMENT_METHOD(Mem, initVBO) { THIS_CHECK;
	REQ_UINT32_ARG(0, vboId);
	cuda_vbo_init(vboId, (void **)&m_devicePtr, &vbo);
	RET_UNDEFINED;
}


JS_IMPLEMENT_METHOD(Mem, regVBO) { THIS_CHECK;
	REQ_UINT32_ARG(0, vboId);
	cuda_vbo_reg(vboId, (void **)&m_devicePtr, &vbo);
	RET_UNDEFINED;
}


JS_IMPLEMENT_METHOD(Mem, unregVBO) { THIS_CHECK;
	cuda_vbo_unreg(&vbo);
	RET_UNDEFINED;
}
