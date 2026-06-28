#include "modulex.hpp"
#include "function.hpp"

#include <fstream>


IMPLEMENT_ES5_CLASS(Modulex);

void Modulex::init(Napi::Env env, Napi::Object exports) {
	
	Napi::Function ctor = wrap(env);
	
	JS_ASSIGN_GETTER(isDestroyed);
	
	JS_ASSIGN_METHOD(destroy);
	
	exports.DefineProperties({
		SF(moduleLoad),
		SF(moduleLoadData),
		SF(moduleRuntimeCompile)
	});
	
	JS_ASSIGN_METHOD(getFunction);
	
	exports.Set("Modulex", ctor);
	
}


Modulex::Modulex(const Napi::CallbackInfo &info) { NAPI_ENV;
	
	super(info);
	
	_isDestroyed = false;
	m_module = nullptr;
	
	Napi::Object that = info.This().As<Napi::Object>();
	
	if (info.Length() == 1) {
		std::string fname = info[0].ToString().Utf8Value();
		CUresult error = cuModuleLoad(&m_module, fname.c_str());
		that.Set("fname", info[0]);
		that.Set("error", JS_NUM(error));
	} else if (info.Length() == 2 ){
		// load ptx from string
		
		std::string fname1 = info[0].ToString().Utf8Value();
		std::string ptx1   = info[1].ToString().Utf8Value();
		
		// Load the generated PTX and get a handle to the SAXPY kernel.
		CUresult error = cuModuleLoadDataEx(&m_module, ptx1.c_str(), 0, 0, 0);
		that.Set("fname", info[0]);
		that.Set("error", JS_NUM(error));
		
		
	} else if (info.Length() > 2) {
		std::string fname = info[0].ToString().Utf8Value();
		std::string saxpy = info[1].ToString().Utf8Value();
		
		nvrtcProgram prog;
		nvrtcCreateProgram(&prog, saxpy.c_str(), fname.c_str(), 0, nullptr, nullptr);
		
		const char *opts[] = {
			"--gpu-architecture=compute_75",
			"--maxrregcount=0",
			"--std=c++11",
			"--use_fast_math"
		};
		
			//"--relocatable-device-code=true"
			// try to fix 511/2047 9/11 PTX cross-gpu problem
			// "--relocatable-device-code=true"
		
		
		// 
		nvrtcResult compileResult = nvrtcCompileProgram( prog, 4, opts );
		
		// nvrtcResult compileResult = nvrtcCompileProgram( prog, 5, opts );
		
		// Obtain compilation log from the program.
		size_t logSize;
		
		nvrtcGetProgramLogSize(prog, &logSize);
		
		char *log = new char[logSize];
		nvrtcGetProgramLog(prog, log);
		that.Set("log", log);
		delete[] log;
		
		if (compileResult == NVRTC_SUCCESS) {
			// Obtain PTX from the program.
			size_t ptxSize;
			nvrtcGetPTXSize(prog, &ptxSize);
			char *ptx = new char[ptxSize];
			nvrtcGetPTX(prog, ptx);
			// Destroy the program.
			nvrtcDestroyProgram(&prog);
			
			std::string dumpto_file = info[2].ToString().Utf8Value();
			std::ofstream outFile;
			outFile.open(dumpto_file.c_str(), std::ios::out | std::ios::binary);
			outFile.write( ptx, ptxSize );
			outFile.close();
			
			// Load the generated PTX and get a handle to the SAXPY kernel.
			CUresult error = cuModuleLoadDataEx(&m_module, ptx, 0, 0, 0);
			that.Set("fname", info[0]);
			that.Set("error", JS_NUM(error));
		};
	}
	
}





Modulex::~Modulex() {
	// CUDA modules are unloaded only by explicit destroy().
	// GC finalization must not invalidate functions created from this module.
}


void Modulex::_destroy() { DES_CHECK;
	
	if (m_module != nullptr) {
		cuModuleUnload(m_module);
		m_module = nullptr;
	}
	_isDestroyed = true;
	
}


JS_IMPLEMENT_METHOD(Modulex, destroy) { THIS_CHECK;
	_destroy();
	RET_UNDEFINED;
}


JS_IMPLEMENT_GETTER(Modulex, isDestroyed) { THIS_CHECK;
	RET_BOOL(_isDestroyed);
}


JS_METHOD(Modulex::moduleLoad) { NAPI_ENV;
	
	napi_value argv[1];
	argv[0] = info[0];
	RET_VALUE(_ctorEs5.Value().New(1, argv));
	
}

JS_METHOD(Modulex::moduleLoadData) { NAPI_ENV;
	
	napi_value argv[2];
	
	argv[0] = info[0];
	argv[1] = info[1];
	
	RET_VALUE(_ctorEs5.Value().New(2, argv));
	
}


JS_METHOD(Modulex::moduleRuntimeCompile) { NAPI_ENV;
	
	napi_value argv[3];
	argv[0] = info[0];
	argv[1] = info[1];
	if (info.Length() > 2) {
		argv[2] = info[2];
	}
	RET_VALUE(_ctorEs5.Value().New(
		info.Length() > 2 ? 3 : 2,
		argv
	));
	
}

JS_IMPLEMENT_METHOD(Modulex, getFunction) { THIS_CHECK;
	
	REQ_STR_ARG(0, name);
	
	CUfunction func;
	CUresult error = cuModuleGetFunction(&func, m_module, name.c_str());
	
	napi_value argv[1];
	argv[0] = JS_EXT(func);
	Napi::Object funcObj = Function::getCtor().New(1, argv);
	funcObj.Set("name", info[0]);
	funcObj.Set("error", JS_NUM(error));
	
	RET_VALUE(funcObj);
	
}
