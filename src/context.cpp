#include "context.h"
#include "utils.h"
#include <boost/algorithm/string.hpp>
#define GEN_LOG_FUNC(FUNC, STREAM)                                      \
    void FUNC(const v8::FunctionCallbackInfo<v8::Value> &args)          \
    {                                                                   \
        v8::Isolate *isolate = args.GetIsolate();                       \
        v8::HandleScope handle_scope(isolate);                          \
        std::vector<std::string> args_list;                             \
        for (int i = 0; i < args.Length(); ++i) {                       \
            v8::String::Utf8Value arg_str(isolate, args[i]);            \
            args_list.push_back(ws2s(s2ws(*arg_str, CP_UTF8), CP_ACP)); \
        }                                                               \
        if (args_list.size() > 0) {                                     \
            STREAM << boost::join(args_list, " ");                      \
        }                                                               \
    }

script_info g_script_info;

GEN_LOG_FUNC(log_error, LOG(ERROR));
GEN_LOG_FUNC(log_warn, LOG(WARNING));
GEN_LOG_FUNC(log_info, LOG(INFO));
GEN_LOG_FUNC(log_debug, VLOG(1));

void getter_callback(v8::Local<v8::String> property,
                     const v8::PropertyCallbackInfo<v8::Value> &info)
{
    v8::Isolate *isolate = info.GetIsolate();
    v8::HandleScope handle_scope(isolate);
    v8::String::Utf8Value property_value(isolate, property);
    std::string property_str = *property_value;
    if (property_str == "__CODE__") {
        info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, g_script_info.code.c_str()));
    } else if (property_str == "__FILE__") {
        info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, g_script_info.filename.c_str()));
    } else if (property_str == "__RPATH__") {
        info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, g_script_info.rpath.c_str()));
    }
}

v8::Local<v8::Context> create_context(v8::Isolate *isolate)
{
    v8::Local<v8::ObjectTemplate> logger = v8::ObjectTemplate::New(isolate);
    logger->Set(isolate, "error", v8::FunctionTemplate::New(isolate, log_error));
    logger->Set(isolate, "warn", v8::FunctionTemplate::New(isolate, log_warn));
    logger->Set(isolate, "info", v8::FunctionTemplate::New(isolate, log_info));
    logger->Set(isolate, "debug", v8::FunctionTemplate::New(isolate, log_debug));

    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    global->Set(isolate, "__LOG__", logger);
    global->SetAccessor(v8::String::NewFromUtf8(isolate, "__CODE__"), getter_callback);
    global->SetAccessor(v8::String::NewFromUtf8(isolate, "__FILE__"), getter_callback);
    global->SetAccessor(v8::String::NewFromUtf8(isolate, "__RPATH__"), getter_callback);
    return node::NewContext(isolate, global);
}
