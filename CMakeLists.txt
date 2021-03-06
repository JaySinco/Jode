cmake_minimum_required(VERSION 3.16)

project(Jode)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include_directories(AFTER
  ${CMAKE_SOURCE_DIR}/src/
  ${CMAKE_SOURCE_DIR}/external/boost/include/boost-1_74/
  ${CMAKE_SOURCE_DIR}/external/fmt/include/
  ${CMAKE_SOURCE_DIR}/external/gflags/include/
  ${CMAKE_SOURCE_DIR}/external/glog/include/
  ${CMAKE_SOURCE_DIR}/external/json/include/
  ${CMAKE_SOURCE_DIR}/external/node/include/
  ${CMAKE_SOURCE_DIR}/external/node/include/v8/
  ${CMAKE_SOURCE_DIR}/external/node/include/uv/
)

link_directories(AFTER
  ${CMAKE_SOURCE_DIR}/external/boost/lib/
  ${CMAKE_SOURCE_DIR}/external/fmt/lib/
  ${CMAKE_SOURCE_DIR}/external/gflags/lib/
  ${CMAKE_SOURCE_DIR}/external/glog/lib/
  ${CMAKE_SOURCE_DIR}/external/node/lib/
)

add_executable(jode
  ${CMAKE_SOURCE_DIR}/src/resource.rc
  ${CMAKE_SOURCE_DIR}/src/platform.cpp
  ${CMAKE_SOURCE_DIR}/src/node_internal.cpp
  ${CMAKE_SOURCE_DIR}/src/context.cpp
  ${CMAKE_SOURCE_DIR}/src/jode.cpp
)

target_link_libraries(jode
  icudata.lib icui18n.lib icustubdata.lib icutools.lib icuucx.lib
  v8_compiler.lib v8_libbase.lib v8_libplatform.lib v8_snapshot.lib
  v8_base_without_compiler.lib v8_libsampler.lib llhttp.lib zlib.lib
  brotli.lib cares.lib histogram.lib http_parser.lib torque_base.lib 
  libnode.lib libuv.lib uvwasi.lib nghttp2.lib openssl.lib winmm.lib
  dbghelp.lib ws2_32.lib crypt32.lib iphlpapi.lib psapi.lib userenv.lib
  fmt.lib gflags_static.lib glog.lib shlwapi.lib
)
