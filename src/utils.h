#pragma once
#include <chrono>
#include <gflags/gflags.h>
#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <fmt/format.h>
#define BOOST_ALL_NO_LIB

using namespace fmt::literals;
using namespace std::chrono_literals;
using namespace std::string_literals;

#define INIT_LOG(argc, argv)                           \
    FLAGS_logtostderr = 1;                             \
    FLAGS_minloglevel = 0;                             \
    gflags::ParseCommandLineFlags(&argc, &argv, true); \
    google::InitGoogleLogging(argv[0]);

namespace utils
{
std::string ws2s(const std::wstring &ws, bool u8_instead_of_ansi = false);
std::wstring s2ws(const std::string &s, bool u8_instead_of_ansi = false);

bool move_item_to_trash(const std::wstring &path);
std::pair<bool, std::string> read_file(const std::wstring &path);
std::tuple<bool, int, std::string> exec(const std::wstring &cmd);
std::tuple<bool, size_t, const char *> load_rc_file(const wchar_t *name);

std::pair<bool, std::wstring> read_clipboard_text();
bool write_clipboard_text(const std::wstring &text);
}  // namespace utils
