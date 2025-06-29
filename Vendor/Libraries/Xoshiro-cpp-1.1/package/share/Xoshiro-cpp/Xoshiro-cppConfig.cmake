cmake_minimum_required (VERSION 3.16)

include(FindPackageHandleStandardArgs)
set(${CMAKE_FIND_PACKAGE_NAME}_CONFIG ${CMAKE_CURRENT_LIST_FILE})
find_package_handle_standard_args(Xoshiro-cpp CONFIG_MODE)

if(NOT TARGET Xoshiro-cpp::Xoshiro-cpp)
  include("${CMAKE_CURRENT_LIST_DIR}/Xoshiro-cppTargets.cmake")
endif()
