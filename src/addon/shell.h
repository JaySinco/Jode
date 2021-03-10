#pragma once
#include <napi.h>

class shell_module: public Napi::ObjectWrap<shell_module>
{
public:
    static Napi::Object init(Napi::Env env, Napi::Object exports);
    shell_module(const Napi::CallbackInfo &info);

private:
    static Napi::FunctionReference constructor;
    static Napi::Value is_file_hidden(const Napi::CallbackInfo &info);
};
