#include "shell.h"

Napi::Object init(Napi::Env env, Napi::Object exports)
{
    shell_t::init(env, exports);
    return exports;
}

NODE_API_MODULE(addon, init)
