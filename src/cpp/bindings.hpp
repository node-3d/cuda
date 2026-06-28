#pragma once

#include <iostream>

#include <addon-tools.hpp>


#define SF(name) Napi::PropertyDescriptor::Function(env, exports, #name, name)

JS_METHOD(driverVersion);
JS_METHOD(deviceCount);
JS_METHOD(glDeviceCount);
