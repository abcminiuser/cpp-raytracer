cmake_minimum_required (VERSION 3.16)

set (CMAKE_CONFIGURATION_TYPES Debug Release)

project ("RayTracer")

add_library(objloader INTERFACE)
target_include_directories (objloader INTERFACE Vendor/OBJ-Loader/Source)
add_library(objloader::objloader ALIAS objloader)

add_subdirectory ("RayTracer")

set (CPACK_PACKAGE_NAME "RayTracer")
set (CPACK_PACKAGE_VERSION "0.0.0")
set (CPACK_PACKAGE_VENDOR "Dean Camera")

include (CPack)
