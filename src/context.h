#pragma once
#include <node.h>

struct script_info
{
    std::string code;
    std::string filename;
    std::string rpath;
};
extern script_info g_script_info;

v8::Local<v8::Context> create_context(v8::Isolate *isolate);
