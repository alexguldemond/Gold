#include <memory>
#include <nan.h>
#include <nan_object_wrap.h>
#include "Gold/math/expression.hpp"

class Expression : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init);

private:
    Expression() { }
    explicit Expression(const std::string& str) : expression(std::make_unique<Gold::math::expression>(str)) {}
    explicit Expression(int value) : expression(std::make_unique<Gold::math::expression>(value)) {}
    explicit Expression(double value) : expression(std::make_unique<Gold::math::expression>(value)) {}
    ~Expression() { }

    static std::map<std::string, Gold::math::expression> v8_object_to_map(v8::Isolate* isolate, const v8::Local<v8::Object>& object);

    static NAN_METHOD(New);
    static NAN_METHOD(toString);
    static NAN_METHOD(defined);
    static NAN_METHOD(evaluate);
    static NAN_METHOD(call);

    static inline Nan::Persistent<v8::Function> & constructor() {
	static Nan::Persistent<v8::Function> my_constructor;
	return my_constructor;
    }

    std::unique_ptr<Gold::math::expression> expression;
};
