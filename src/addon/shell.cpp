#include "shell.h"
#include "utils.h"

Napi::FunctionReference ShellModule::constructor;

ShellModule::ShellModule(const Napi::CallbackInfo &info): Napi::ObjectWrap<ShellModule>(info) {}

Napi::Object ShellModule::init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func =
        DefineClass(env, "shell",
                    {
                        StaticMethod<&ShellModule::moveItemToTrash>("moveItemToTrash"),
                        StaticMethod<&ShellModule::readClipboard>("readClipboard"),
                        StaticMethod<&ShellModule::writeClipboard>("writeClipboard"),
                    });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("shell", func);
    return exports;
}

class MoveItemToTrashWorker: public Napi::AsyncWorker
{
public:
    MoveItemToTrashWorker(Napi::Env &env, Napi::Promise::Deferred &deferred,
                          const std::wstring &path)
        : Napi::AsyncWorker(env), deferred_(deferred), path_(path)
    {
    }

    ~MoveItemToTrashWorker() {}

    void Execute() { ok = utils::move_item_to_trash(path_); }

    void OnOK() { deferred_.Resolve(Napi::Boolean::New(Env(), ok)); }

    void OnError(const Napi::Error &error) { deferred_.Reject(error.Value()); }

private:
    Napi::Promise::Deferred deferred_;
    std::wstring path_;
    bool ok;
};

Napi::Value ShellModule::moveItemToTrash(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() != 1 || !info[0].IsString()) {
        Napi::TypeError::New(
            env, "invalid argument, should be moveItemToTrash(path: string) => Promise<boolean>")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    std::string path = info[0].As<Napi::String>();
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    auto worker = new MoveItemToTrashWorker(env, deferred, utils::s2ws(path, true));
    worker->Queue();
    return deferred.Promise();
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
