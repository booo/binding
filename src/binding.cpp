#include <v8.h>
#include <node.h>

using namespace v8;
using namespace node;

static Handle<Value> TheFunction(const Arguments& args) {
    return True();
}

void Initialize(Handle<Object> target) {
    HandleScope scope;

    //et attribute version (String) of module
    target->Set(String::NewSymbol("version"), String::NewSymbol("0.0.1"));

    //export a Number
    target->Set(String::NewSymbol("PI"), Number::New(3.14159265));

    //export an Array
    Local<Array> array = Array::New(4);
    //fill the array with objects
    array->Set(0,True());
    array->Set(1,False());
    array->Set(2,Null());
    array->Set(3,Undefined());
    target->Set(String::NewSymbol("theArray"), array);

    //export an Object
    Local<Object> object = Object::New();
    target->Set(String::NewSymbol("theObject"), object);

    //export a function
    Local<FunctionTemplate> t = FunctionTemplate::New(TheFunction);
    target->Set(String::NewSymbol("theFunction"), t->GetFunction());

    //alias for defining a function
    NODE_SET_METHOD(target,"anotherFunction", TheFunction);

}

extern "C" {
    void init(Handle<Object> target) {
        HandleScope scope;
        Initialize(target);
    }
    NODE_MODULE(binding, init);
}
