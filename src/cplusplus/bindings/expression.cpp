#include "Gold/bindings/expression.hpp"
#include "v8pp/convert.hpp"

std::map<std::string, Gold::math::expression> Expression::v8_object_to_map(v8::Isolate* isolate, const v8::Local<v8::Object>& object) {
    std::map<std::string, Gold::math::expression> map;
    v8::Local<v8::Array> keys = object->GetPropertyNames();
    for (uint i = 0; i < keys->Length(); i++) {
	auto v8_key = keys->Get(i);
	std::string key = v8pp::from_v8<std::string>(isolate, v8_key->ToString());
	
	Gold::math::expression value;
	
	if ( object->Get(v8_key)->IsString() ) {
	    try {
		std::string str = v8pp::from_v8<std::string>(isolate, object->Get(v8_key)->ToString());
		value = Gold::math::expression(str);
	    }
	    catch (std::string& e) {
		Nan::ThrowError(e.c_str());
	    }
	}
	else if ( object->Get(v8_key)->IsInt32() ) {
	    Nan::Maybe<int32_t> maybe_num = Nan::To<int32_t>(object->Get(v8_key));
	    if ( maybe_num.IsNothing() ) {
		Nan::ThrowTypeError("Not a number");
		return map;
	    }
	    int num = maybe_num.FromJust();
	    try {
		value = Gold::math::expression(num);
	    }
	    catch (std::string& e) {
		Nan::ThrowError(e.c_str());
	    }
	}
	else if ( object->Get(v8_key)->IsNumber() ) {
	    Nan::Maybe<double> maybe_num = Nan::To<double>(object->Get(v8_key));
	    if ( maybe_num.IsNothing() ) {
		Nan::ThrowTypeError("Not a number");
		return map;
	    }
	    double num = maybe_num.FromJust();
	    try {
		value = Gold::math::expression(num);
	    }
	    catch (std::string& e) {
		Nan::ThrowError(e.c_str());
	    }
	}
	else if (object->Get(v8_key)->IsObject() ){
	    v8::Local<v8::Object> wrapped = object->Get(v8_key)->ToObject();
	    Expression* expr = Nan::ObjectWrap::Unwrap<Expression>(wrapped);
	    value = *(expr->expression);
	}
	else {
	    Nan::ThrowTypeError("Invalid type passed in");
	}
	map.insert(std::pair<std::string, Gold::math::expression>(key, value));
    }
    return map;
}



NAN_MODULE_INIT(Expression::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("Expression").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetCallAsFunctionHandler(tpl->InstanceTemplate(), call);
    Nan::SetPrototypeMethod(tpl, "toString", toString);
    Nan::SetPrototypeMethod(tpl, "defined", defined);
    Nan::SetPrototypeMethod(tpl, "evaluate", evaluate);

    constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
    Nan::Set(target, Nan::New("Expression").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(Expression::New) {
    Expression* expression;
    if (info.IsConstructCall()) {
	if (info.Length() == 0) {
	    expression = new Expression;
	}
	else if (info[0]->IsString()) {
	    v8::String::Utf8Value param1(info[0]->ToString());
	    std::string str = std::string(*param1);
	    expression = new Expression(str);
	}
	else if (info[0]->IsInt32()) {
	    expression = new Expression((int)info[0]->IntegerValue());
	}
	else if (info[0]->IsNumber()) {
	    expression = new Expression(info[0]->NumberValue());
	}
	else {
	    Nan::ThrowTypeError("Wrong arguments");
	    return;
	}
	expression->Wrap(info.This());
	info.GetReturnValue().Set(info.This());
    }
    else {
	const int argc = 1;
	v8::Local<v8::Value> argv[argc] = {info[0]};
	v8::Local<v8::Function> cons = Nan::New(constructor());
	Nan::MaybeLocal<v8::Object> maybeInstance = Nan::NewInstance(cons, argc, argv);
	v8::Local<v8::Object> instance;
	if (maybeInstance.IsEmpty()) {
	    Nan::ThrowError("Could not create new Expression instance");
	} else {
	    instance = maybeInstance.ToLocalChecked();
	    info.GetReturnValue().Set(instance);
	}
    }
}

NAN_METHOD(Expression::toString) {
    v8::Isolate* isolate = info.GetIsolate();
    Expression* expression = ObjectWrap::Unwrap<Expression>(info.Holder());
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, expression->expression->string().c_str()));
}

NAN_METHOD(Expression::defined) {
    v8::Isolate* isolate = info.GetIsolate();
    Expression* expression = ObjectWrap::Unwrap<Expression>(info.Holder());
    info.GetReturnValue().Set(v8::Boolean::New(isolate, expression->expression->defined()));
}

NAN_METHOD(Expression::evaluate) {
    v8::Isolate* isolate = info.GetIsolate();
    Expression* expression = ObjectWrap::Unwrap<Expression>(info.Holder());
    if (info.Length() > 0 && !info[0]->IsObject()) {
	isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong arguments: Takes object")));
	return;	
    }
    
    v8::Local<v8::Object> object = info[0]->ToObject();
    std::map<std::string, double> map;
    try {
	map = v8pp::from_v8<std::map<std::string, double> >(isolate, object);
    }
    catch (std::invalid_argument& e) {
	isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, e.what())));
	return;
    }
    double return_value;
    try {
	return_value = expression->expression->evaluate(map);
    }
    catch (std::string& e) {
	isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, e.c_str())));
	return;
    }
    info.GetReturnValue().Set(return_value);
}

NAN_METHOD(Expression::call) {
    v8::Isolate* isolate = info.GetIsolate();
    Expression* expression = ObjectWrap::Unwrap<Expression>(info.Holder());
    Gold::math::expression new_gold_expression;
    
    if (info.Length() == 0 ) {
	try {
	    new_gold_expression = (*expression->expression)();
	}
	catch (std::string& e) {
	    Nan::ThrowError(e.c_str());
	    return;
	}
    }
    else if ( info[0]->IsObject() ){
	try {
	    new_gold_expression = (*expression->expression)(v8_object_to_map(isolate, info[0]->ToObject()));
	}
	catch (std::string& e) {
	    Nan::ThrowError(e.c_str());
	    return;
	}
    }
    else {
	Nan::ThrowTypeError("Invoking Expressions takes an object of strings and Expressions");
    }
    v8::Local<v8::Function> cons = Nan::New(constructor());
    Nan::MaybeLocal<v8::Object> maybeInstance = Nan::NewInstance(cons);
    v8::Local<v8::Object> instance;
    if (maybeInstance.IsEmpty()) {
	Nan::ThrowError("Could not create new Expression instance");
    } else {
	instance = maybeInstance.ToLocalChecked();
	Expression* new_expression = ObjectWrap::Unwrap<Expression>(instance);
	new_expression->expression = std::make_unique<Gold::math::expression>(std::move(new_gold_expression));
	info.GetReturnValue().Set(instance);
    }
}

NODE_MODULE(binding, Expression::Init);
