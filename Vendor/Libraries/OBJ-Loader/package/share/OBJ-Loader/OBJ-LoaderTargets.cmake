cmake_minimum_required (VERSION 3.16)

add_library(OBJ-Loader INTERFACE)
target_include_directories (OBJ-Loader INTERFACE ${CMAKE_CURRENT_LIST_DIR}/../../../Source)

add_library(OBJ-Loader::OBJ-Loader ALIAS OBJ-Loader)
