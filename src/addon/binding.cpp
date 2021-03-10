#include "utils.h"
#include <napi.h>

Napi::Boolean isFileHidden(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() != 1 || (!info[0].IsBuffer() && !info[0].IsString())) {
        Napi::TypeError::New(env, "invalid argument, should be (path: string|Buffer) => boolean")
            .ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }
    std::string u8path;
    if (info[0].IsString()) {
        u8path = info[0].As<Napi::String>();
    }
    if (info[0].IsBuffer()) {
        Napi::Buffer<char> buffer = info[0].As<Napi::Buffer<char>>();
        u8path = std::string(buffer.Data(), buffer.Length());
    }
    std::wstring path = s2ws(u8path, CP_UTF8);
    DWORD attr = GetFileAttributesW(path.c_str());
    if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_HIDDEN)) {
        return Napi::Boolean::New(env, true);
    }
    return Napi::Boolean::New(env, false);
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports)
{
    exports.Set("isFileHidden", Napi::Function::New(env, isFileHidden));
    return exports;
}

NODE_API_MODULE(addon, InitAll)
