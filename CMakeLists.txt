cmake_minimum_required(VERSION 3.16)

project(Jode)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include_directories(AFTER
  ${CMAKE_SOURCE_DIR}/src/
  ${CMAKE_SOURCE_DIR}/deps/boost/include/boost-1_74/
  ${CMAKE_SOURCE_DIR}/deps/fmt/include/
  ${CMAKE_SOURCE_DIR}/deps/gflags/include/
  ${CMAKE_SOURCE_DIR}/deps/glog/include/
  ${CMAKE_SOURCE_DIR}/deps/json/include/
  ${CMAKE_SOURCE_DIR}/deps/node/include/
  ${CMAKE_SOURCE_DIR}/deps/node/include/v8/
  ${CMAKE_SOURCE_DIR}/deps/node/include/uv/
  ${CMAKE_SOURCE_DIR}/deps/node-addon-api/include/
)

link_directories(AFTER
  ${CMAKE_SOURCE_DIR}/deps/boost/lib/
  ${CMAKE_SOURCE_DIR}/deps/fmt/lib/
  ${CMAKE_SOURCE_DIR}/deps/gflags/lib/
  ${CMAKE_SOURCE_DIR}/deps/glog/lib/
  ${CMAKE_SOURCE_DIR}/deps/node/lib/
)

add_library(common STATIC ${CMAKE_SOURCE_DIR}/src/platform.cpp)
target_link_libraries(common fmt.lib gflags_static.lib glog.lib shlwapi.lib)

add_executable(jode
  ${CMAKE_SOURCE_DIR}/src/res/jode.rc
  ${CMAKE_SOURCE_DIR}/src/node_internal.cpp
  ${CMAKE_SOURCE_DIR}/src/context.cpp
  ${CMAKE_SOURCE_DIR}/src/jode.cpp
)
target_link_libraries(jode common
  icudata.lib icui18n.lib icustubdata.lib icutools.lib icuucx.lib v8_compiler.lib
  v8_libbase.lib llhttp.lib zlib.lib libuv.lib uvwasi.lib psapi.lib userenv.lib
  v8_libplatform.lib v8_snapshot.lib v8_base_without_compiler.lib v8_libsampler.lib
  brotli.lib cares.lib histogram.lib http_parser.lib torque_base.lib libnode.lib
  nghttp2.lib openssl.lib winmm.lib dbghelp.lib ws2_32.lib crypt32.lib iphlpapi.lib
)

add_library(addon SHARED
  ${CMAKE_SOURCE_DIR}/src/addon/delay_load_hook.cpp
  ${CMAKE_SOURCE_DIR}/src/addon/binding.cpp
)
target_compile_definitions(addon PUBLIC
  NAPI_VERSION=3
  BUILDING_NODE_EXTENSION
  NAPI_DISABLE_CPP_EXCEPTIONS
)
set_target_properties(addon PROPERTIES LINK_FLAGS "/DELAYLOAD:node.exe")
set_target_properties(addon PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(addon common node.lib)

add_custom_target(webpack ALL DEPENDS addon
  COMMAND yarn run webpack --config ${CMAKE_SOURCE_DIR}/lib/config/webpack.config.js
)
