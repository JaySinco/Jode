#include "platform.h"
#include <napi.h>

Napi::Number getAllocationSize(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() != 1 || (!info[0].IsBuffer() && !info[0].IsString())) {
        Napi::TypeError::New(env, "argument (filePath: string|Buffer) expected")
            .ThrowAsJavaScriptException();
        return Napi::Number::New(env, 0);
    }
    std::string filePath;
    if (info[0].IsString()) {
        filePath = info[0].As<Napi::String>();
    }
    if (info[0].IsBuffer()) {
        Napi::Buffer<char> buffer = info[0].As<Napi::Buffer<char>>();
        filePath = std::string(buffer.Data(), buffer.Length());
    }
    HANDLE hFile = ::CreateFileA(filePath.c_str(), GENERIC_READ | FILE_SHARE_READ, 0, NULL,
                                 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        FILE_STANDARD_INFO fsi = {0};
        if (::GetFileInformationByHandleEx(hFile, FileStandardInfo, &fsi,
                                           sizeof(FILE_STANDARD_INFO))) {
            LARGE_INTEGER size = fsi.AllocationSize;
            CloseHandle(hFile);
            return Napi::Number::New(env, size.QuadPart);
        }
        CloseHandle(hFile);
    }
    return Napi::Number::New(env, 0);
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports)
{
    exports.Set("getAllocationSize", Napi::Function::New(env, getAllocationSize));
    return exports;
}

NODE_API_MODULE(addon, InitAll)
