#include "context.h"
#include "platform.h"
#include <boost/algorithm/string.hpp>

injected_t g_injected;

#define MAKE_LOG(func, ostream)                                       \
    static void func(const v8::FunctionCallbackInfo<v8::Value> &args) \
    {                                                                 \
        v8::Isolate *isolate = args.GetIsolate();                     \
        v8::HandleScope handle_scope(isolate);                        \
        std::vector<std::string> args_list;                           \
        for (int i = 0; i < args.Length(); ++i) {                     \
            v8::String::Utf8Value arg_str(isolate, args[i]);          \
            args_list.push_back(ws2s(s2ws(*arg_str, CP_UTF8)));       \
        }                                                             \
        if (args_list.size() > 0) {                                   \
            ostream << boost::join(args_list, " ");                   \
        }                                                             \
    }

MAKE_LOG(log_error, LOG(ERROR));
MAKE_LOG(log_warn, LOG(WARNING));
MAKE_LOG(log_info, LOG(INFO));
MAKE_LOG(log_debug, VLOG(1));

static void global_getter_cb(v8::Local<v8::String> property,
                             const v8::PropertyCallbackInfo<v8::Value> &info)
{
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope handle_scope(isolate);
    v8::String::Utf8Value property_value(isolate, property);
    std::string property_str = *property_value;
    if (property_str == "__rpath") {
        info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, g_injected.rpath.c_str()));
    } else if (property_str == "__filename") {
        info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, g_injected.filename.c_str()));
    } else if (property_str == "__dirname") {
        info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, g_injected.dirname.c_str()));
    } else if (property_str == "__code") {
        info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, g_injected.code.c_str()));
    }
}

v8::Local<v8::Context> create_context(v8::Isolate *isolate)
{
    v8::Local<v8::ObjectTemplate> log = v8::ObjectTemplate::New(isolate);
    log->Set(isolate, "error", v8::FunctionTemplate::New(isolate, log_error));
    log->Set(isolate, "warn", v8::FunctionTemplate::New(isolate, log_warn));
    log->Set(isolate, "info", v8::FunctionTemplate::New(isolate, log_info));
    log->Set(isolate, "debug", v8::FunctionTemplate::New(isolate, log_debug));

    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    global->Set(isolate, "log", log);
    global->SetAccessor(v8::String::NewFromUtf8(isolate, "__rpath"), global_getter_cb);
    global->SetAccessor(v8::String::NewFromUtf8(isolate, "__filename"), global_getter_cb);
    global->SetAccessor(v8::String::NewFromUtf8(isolate, "__dirname"), global_getter_cb);
    global->SetAccessor(v8::String::NewFromUtf8(isolate, "__code"), global_getter_cb);
    return node::NewContext(isolate, global);
}
