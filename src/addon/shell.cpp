#include "shell.h"
#include "utils.h"

Napi::FunctionReference ShellModule::constructor;

ShellModule::ShellModule(const Napi::CallbackInfo &info): Napi::ObjectWrap<ShellModule>(info) {}

Napi::Object ShellModule::init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func =
        DefineClass(env, "shell",
                    {
                        StaticMethod<&ShellModule::readClipboard>("readClipboard"),
                        StaticMethod<&ShellModule::writeClipboard>("writeClipboard"),
                    });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("shell", func);
    return exports;
}

Napi::Value ShellModule::readClipboard(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() != 0) {
        Napi::TypeError::New(env,
                             "invalid argument, should be readClipboard() => [boolean, string]")
            .ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    auto [ok, text] = utils::read_clipboard_text();
    Napi::Array arr = Napi::Array::New(env, 2);
    arr[static_cast<uint32_t>(0)] = Napi::Boolean::New(env, ok);
    arr[1] = Napi::String::New(env, utils::ws2s(text, true));
    return arr;
}

Napi::Value ShellModule::writeClipboard(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() != 1 || !info[0].IsString()) {
        Napi::TypeError::New(env,
                             "invalid argument, should be writeClipboard(text: string) => boolean")
            .ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }
    std::string text = info[0].As<Napi::String>();
    return Napi::Boolean::New(env, utils::write_clipboard_text(utils::s2ws(text, true)));
}
