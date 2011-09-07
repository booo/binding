#include <v8.h/
#include <node.h>

using namespace v8;
using namespace node;

typedef struct {
    int fac;
    int n;
    Persistent<Function> cb;
} fac_baton_t;

class Apple: ObjectWrap {
    private:
        bool eaten;
    public:
        Apple() {
            eaten = false;
        }

        ~Apple() {
        }

        static Handle<Value> New(const Arguments& args) {
            HandleScope scope;
            Apple* apple = new Apple();
            apple->Wrap(args.This());
            return args.This();
        }
        static Handle<Value> Eat(const Arguments& args) {
            HandleScope scope;
            Apple* apple = ObjectWrap::Unwrap<Apple>(args.This());
            apple->eaten = true;
            return True();
        }
};


int EIO_Fac(eio_req *req) {
    fac_baton_t *closure = static_cast<fac_baton_t *>(req->data);
    int i;

    closure->fac = 1;

    for(i=1; i<closure->n;i++) {
        closure->fac = i*closure->fac;
    }

    return 0;
}

int EIO_AfterFac(eio_req *req) {

    HandleScope scope;

    fac_baton_t *closure = static_cast<fac_baton_t *>(req->data);

    TryCatch tryCatch;

    Local<Value> argv[1] = { Local<Value>::New(Number::New(closure->fac)) };

    closure->cb->Call(Context::GetCurrent()->Global(),1,argv);

    if(tryCatch.HasCaught()) {
        FatalException(tryCatch);
    }

    ev_unref(EV_DEFAULT_UC); //check this function!
    closure->cb.Dispose();
    delete closure;
    return 0;
}

static Handle<Value> AsyncFunction(const Arguments& args) {
    HandleScope scope;

    fac_baton_t *closure = new fac_baton_t();
    closure->n = args[0]->IntegerValue();
    closure->cb = Persistent<Function>::New(Handle<Function>::Cast(args[1]));
    eio_custom(EIO_Fac, EIO_PRI_DEFAULT, EIO_AfterFac, closure);

    ev_ref(EV_DEFAULT_UC);

    return Undefined();
}

static Handle<Value> FunctionWithCallback(const Arguments& args) {
    HandleScope scope;

    TryCatch try_catch;

    Local<Value> argv[1] = { Local<Value>::New(Number::New(42)) };

    Local<Function>::New(Handle<Function>::Cast(args[0]))->Call(Context::GetCurrent()->Global(),1,argv);

    if (try_catch.HasCaught()) {
        FatalException(try_catch);
    }
    return Undefined();
}

static Handle<Value> Add(const Arguments& args) {

    if(args.Length() >= 2 && args[0]->IsNumber() && args[1]->IsNumber()) {
        return Number::New(args[0]->NumberValue() + args[1]->NumberValue());
    }
    else {
        return ThrowException(String::New("Need two arguments of type number."));
    }
}

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

    //export a function, do some input parsing
    NODE_SET_METHOD(target, "add", Add);

    //a function with callback
    NODE_SET_METHOD(target, "functionWithCallback", FunctionWithCallback);

    //a async function
    NODE_SET_METHOD(target, "asyncFunction", AsyncFunction);

    //prototyping
    Local<FunctionTemplate> t2 = FunctionTemplate::New();
    Local<Template> proto_t = t2->PrototypeTemplate();
    proto_t->Set(String::New("wheeles"), Number::New(2));

    target->Set(String::NewSymbol("Bike"), t2->GetFunction());

    //Object wrapping

    Local<FunctionTemplate> apple_t = FunctionTemplate::New(Apple::New);
    apple_t->InstanceTemplate()->SetInternalFieldCount(1);
    apple_t->SetClassName(String::NewSymbol("Apple"));

    NODE_SET_PROTOTYPE_METHOD(apple_t, "eat", Apple::Eat);

    target->Set(String::NewSymbol("Apple"), apple_t->GetFunction());

}

extern "C" {

    void init(Handle<Object> target) {
        HandleScope scope;
        Initialize(target);
    }
    NODE_MODULE(binding, init);

}
