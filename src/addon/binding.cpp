#include "shell.h"

Napi::Object init(Napi::Env env, Napi::Object exports)
{
    shell_module::init(env, exports);
    return exports;
}

NODE_API_MODULE(addon, init)
