#include "shell.h"
#include "utils.h"

Napi::Object init(Napi::Env env, Napi::Object exports)
{
    google::InitGoogleLogging("");
    ShellModule::init(env, exports);
    return exports;
}

NODE_API_MODULE(addon, init)
