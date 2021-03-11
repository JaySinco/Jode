#define UNICODE
#include <windows.h>
#include "utils.h"
#include <shlobj.h>
#include <wrl/client.h>
#include <filesystem>
#include <fstream>

#define LOG_WINERR(msg) LOG(ERROR) << fmt::format(msg ": {}", GetLastError());

namespace utils
{
std::string ws2s(const std::wstring &ws, bool u8_instead_of_ansi)
{
    UINT page = u8_instead_of_ansi ? CP_UTF8 : CP_ACP;
    int len = WideCharToMultiByte(page, 0, ws.c_str(), -1, nullptr, 0, nullptr, nullptr);
    auto buf = new char[len]{0};
    WideCharToMultiByte(page, 0, ws.c_str(), -1, buf, len, nullptr, nullptr);
    std::string s = buf;
    delete[] buf;
    return s;
}

std::wstring s2ws(const std::string &s, bool u8_instead_of_ansi)
{
    UINT page = u8_instead_of_ansi ? CP_UTF8 : CP_ACP;
    int len = MultiByteToWideChar(page, 0, s.c_str(), -1, nullptr, 0);
    auto buf = new wchar_t[len]{0};
    MultiByteToWideChar(page, 0, s.c_str(), -1, buf, len);
    std::wstring ws = buf;
    delete[] buf;
    return ws;
}

std::pair<bool, std::string> read_file(const std::wstring &path)
{
    std::ifstream in_file(path);
    if (!in_file) {
        LOG(ERROR) << "failed to read file: {}"_format(ws2s(path));
        return {false, ""};
    }
    std::stringstream ss;
    ss << in_file.rdbuf();
    return {true, ss.str()};
}

std::tuple<bool, size_t, const char *> load_rc_file(const wchar_t *name)
{
    HMODULE handle = GetModuleHandle(NULL);
    HRSRC rc = FindResourceW(handle, name, RT_RCDATA);
    if (rc == NULL) {
        LOG_WINERR("failed to find resource");
        return {false, 0, nullptr};
    }
    HGLOBAL rc_data = LoadResource(handle, rc);
    if (rc_data == NULL) {
        LOG_WINERR("failed to load resource");
        return {false, 0, nullptr};
    }
    return {true, SizeofResource(handle, rc), static_cast<const char *>(LockResource(rc_data))};
}

std::tuple<bool, int, std::string> exec(const std::wstring &cmd)
{
    SECURITY_ATTRIBUTES sa = {0};
    sa.bInheritHandle = TRUE;
    sa.nLength = sizeof(sa);
    HANDLE hr = NULL;
    HANDLE hw = NULL;
    if (!CreatePipe(&hr, &hw, &sa, 0)) {
        LOG_WINERR("failed to create pipe")
        return {false, 0, ""};
    }
    SetHandleInformation(hr, HANDLE_FLAG_INHERIT, 0);
    STARTUPINFOW si = {sizeof(si)};
    si.dwFlags = STARTF_FORCEOFFFEEDBACK | STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdError = hw;
    si.hStdOutput = hw;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    PROCESS_INFORMATION pi = {0};
    if (!CreateProcessW(NULL, (LPWSTR)cmd.c_str(), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL,
                        &si, &pi)) {
        CloseHandle(hr);
        CloseHandle(hw);
        LOG_WINERR("failed to create process")
        return {false, 0, ""};
    }
    CloseHandle(hw);
    std::string result;
    const int bsize = 1024;
    char buf[bsize] = {0};
    for (;;) {
        DWORD n_read = 0;
        BOOL ok = ReadFile(hr, buf, bsize, &n_read, NULL);
        if (!ok || n_read == 0) {
            break;
        }
        result.append(buf, n_read);
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(hr);
    DWORD exit_code = 0;
    GetExitCodeProcess(pi.hProcess, &exit_code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return {true, exit_code, result};
}

bool move_item_to_trash(const std::wstring &path)
{
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    std::shared_ptr<void> com_guard(nullptr, [](void *) { CoUninitialize(); });
    Microsoft::WRL::ComPtr<IFileOperation> pfo;
    if (FAILED(CoCreateInstance(CLSID_FileOperation, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pfo)))) {
        LOG(ERROR) << "failed to create file op instance";
        return false;
    }
    if (FAILED(pfo->SetOperationFlags(FOF_NO_UI | FOF_ALLOWUNDO | FOF_NOERRORUI | FOF_SILENT |
                                      FOFX_SHOWELEVATIONPROMPT))) {
        LOG(ERROR) << "failed to set op flags";
        return false;
    }
    Microsoft::WRL::ComPtr<IShellItem> delete_item;
    std::wstring abs_path = std::filesystem::absolute(path).wstring();
    if (FAILED(SHCreateItemFromParsingName(abs_path.c_str(), NULL,
                                           IID_PPV_ARGS(delete_item.GetAddressOf())))) {
        LOG(ERROR) << "failed to create shell item from path: " << utils::ws2s(abs_path);
        return false;
    }
    BOOL pfAnyOperationsAborted;
    if (!SUCCEEDED(pfo->DeleteItem(delete_item.Get(), nullptr))) {
        LOG(ERROR) << "failed to enqueue delete item op";
        return false;
    }
    if (!SUCCEEDED(pfo->PerformOperations())) {
        LOG(ERROR) << "failed to perform delete op";
        return false;
    }
    if (!SUCCEEDED(pfo->GetAnyOperationsAborted(&pfAnyOperationsAborted))) {
        LOG(ERROR) << "failed to check op status";
        return false;
    }
    if (pfAnyOperationsAborted) {
        LOG(ERROR) << "delete op was aborted";
        return false;
    }
    return true;
}

std::pair<bool, std::wstring> read_clipboard_text()
{
    if (!OpenClipboard(NULL)) {
        LOG_WINERR("failed to open clipboard");
        return {false, L""};
    }
    std::shared_ptr<void> clipboard_guard(nullptr, [](void *) { CloseClipboard(); });
    HANDLE handle = GetClipboardData(CF_UNICODETEXT);
    if (handle == nullptr) {
        LOG_WINERR("failed to get clipboard data as text");
        return {false, L""};
    }
    wchar_t *text = static_cast<wchar_t *>(GlobalLock(handle));
    std::shared_ptr<void> glock_guard(nullptr, [=](void *) { GlobalUnlock(handle); });
    if (text == nullptr) {
        LOG_WINERR("empty clipboard data");
        return {false, L""};
    }
    return {true, text};
}

bool write_clipboard_text(const std::wstring &text)
{
    if (!OpenClipboard(NULL)) {
        LOG_WINERR("failed to open clipboard");
        return false;
    }
    std::shared_ptr<void> clipboard_guard(nullptr, [](void *) { CloseClipboard(); });
    if (!EmptyClipboard()) {
        LOG_WINERR("failed to empty clipboard");
        return false;
    }
    HGLOBAL hglb = GlobalAlloc(GMEM_MOVEABLE, (text.size() + 1) * sizeof(wchar_t));
    if (hglb == NULL) {
        LOG_WINERR("failed to allocate memory from heap");
        return false;
    }
    wchar_t *buf = static_cast<wchar_t *>(GlobalLock(hglb));
    std::shared_ptr<void> glock_guard(nullptr, [=](void *) { GlobalUnlock(hglb); });
    memcpy(buf, text.data(), text.size() * sizeof(wchar_t));
    buf[text.size()] = 0;
    if (SetClipboardData(CF_UNICODETEXT, hglb) == NULL) {
        LOG_WINERR("failed to set clipboard data as text");
        return false;
    }
    return true;
}

}  // namespace utils
