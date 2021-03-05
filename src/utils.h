#pragma once
#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define BOOST_ALL_NO_LIB
#include <json.hpp>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <fmt/format.h>
#include <gflags/gflags.h>
#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include <chrono>

using namespace fmt::literals;
using namespace std::chrono_literals;
using namespace std::string_literals;
using json = nlohmann::ordered_json;

#define JD_TRY try {
#define JD_CATCH \
    }            \
    catch (const std::exception &e) { LOG(ERROR) << e.what(); }

#define INIT_LOG(argc, argv)                           \
    FLAGS_logtostderr = 1;                             \
    FLAGS_minloglevel = 0;                             \
    gflags::ParseCommandLineFlags(&argc, &argv, true); \
    google::InitGoogleLogging(argv[0]);

std::string ws2s(const std::wstring &ws, UINT page = CP_ACP);
std::wstring s2ws(const std::string &s, UINT page = CP_ACP);
std::string read_file(const std::wstring &path);
std::pair<int, std::string> exec(const std::wstring &cmd);
