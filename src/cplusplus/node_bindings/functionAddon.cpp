#include <node.h>
#include "function.hpp"

using v8::Local;
using v8::Object;

void InitAll(Local<Object> exports) {
    function::Init(exports);
}

NODE_MODULE(functionAddon, InitAll)

