cmake_minimum_required(VERSION 3.16)

project(Jode)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include_directories(
  ${CMAKE_SOURCE_DIR}/src/
  ${CMAKE_SOURCE_DIR}/external/boost/include/boost-1_74/
  ${CMAKE_SOURCE_DIR}/external/fmt/include/
  ${CMAKE_SOURCE_DIR}/external/gflags/include/
  ${CMAKE_SOURCE_DIR}/external/glog/include/
  ${CMAKE_SOURCE_DIR}/external/json/include/
)

link_directories(
  ${CMAKE_SOURCE_DIR}/external/boost/lib/
  ${CMAKE_SOURCE_DIR}/external/fmt/lib/
  ${CMAKE_SOURCE_DIR}/external/gflags/lib/
  ${CMAKE_SOURCE_DIR}/external/glog/lib/
)

add_library(common STATIC ${CMAKE_SOURCE_DIR}/src/common.cpp)
target_link_libraries(common fmt.lib gflags_static.lib glog.lib)
