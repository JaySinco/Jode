#pragma once
#include <napi.h>

class ShellModule: public Napi::ObjectWrap<ShellModule>
{
public:
    static Napi::Object init(Napi::Env env, Napi::Object exports);
    ShellModule(const Napi::CallbackInfo &info);

private:
    static Napi::FunctionReference constructor;
    static Napi::Value moveItemToTrash(const Napi::CallbackInfo &info);
    static Napi::Value readClipboard(const Napi::CallbackInfo &info);
    static Napi::Value writeClipboard(const Napi::CallbackInfo &info);
};
