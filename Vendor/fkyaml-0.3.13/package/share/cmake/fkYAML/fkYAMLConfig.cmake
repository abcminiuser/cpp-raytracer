include(FindPackageHandleStandardArgs)
set(${CMAKE_FIND_PACKAGE_NAME}_CONFIG ${CMAKE_CURRENT_LIST_FILE})
find_package_handle_standard_args(fkYAML CONFIG_MODE)

if(NOT TARGET fkYAML::fkYAML)
  include("${CMAKE_CURRENT_LIST_DIR}/fkYAMLTargets.cmake")
endif()
