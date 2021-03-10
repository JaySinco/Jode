#pragma once
#include <napi.h>

class shell_t: public Napi::ObjectWrap<shell_t>
{
public:
    static Napi::Object init(Napi::Env env, Napi::Object exports);
    shell_t(const Napi::CallbackInfo &info);

private:
    static Napi::FunctionReference constructor;
    static Napi::Value is_file_hidden(const Napi::CallbackInfo &info);
};
