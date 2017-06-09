#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <node.h>
#include <node_object_wrap.h>
#include <memory>
#include "Gold/math/function.hpp"

class function : public node::ObjectWrap {
public:
    static void Init(v8::Local<v8::Object> exports);

private:
    std::unique_ptr<function> func;

    function(const variable& var, const expression& expr);
    ~function();
    
    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
    
};

#endif
