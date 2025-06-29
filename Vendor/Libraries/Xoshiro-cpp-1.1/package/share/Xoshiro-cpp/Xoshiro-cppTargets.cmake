cmake_minimum_required (VERSION 3.16)

add_library(Xoshiro-cpp INTERFACE)
target_include_directories (Xoshiro-cpp INTERFACE ${CMAKE_CURRENT_LIST_DIR}/../../..)

add_library(Xoshiro-cpp::Xoshiro-cpp ALIAS Xoshiro-cpp)
