#pragma once
#include <node.h>

struct injected_t
{
    std::string code;
    std::string filename;
    std::string rpath;
};

extern injected_t g_injected;

v8::Local<v8::Context> create_context(v8::Isolate *isolate);
