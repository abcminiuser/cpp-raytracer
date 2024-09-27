cmake_minimum_required (VERSION 3.16)

set (VENDOR_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor")

if (MSVC)
    list (APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/Vendor)
    set(SFML_STATIC_LIBRARIES TRUE)
endif ()

find_package (SFML CONFIG REQUIRED COMPONENTS graphics)

add_executable (RayTracer
    "Main.cpp"
	"Engine/Renderer.cpp"
    "Engine/Camera.cpp"
    "Engine/Ray.cpp"
    "Engine/Sphere.cpp"
)

target_include_directories (RayTracer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (RayTracer PRIVATE sfml-graphics)
target_compile_features (RayTracer PUBLIC cxx_std_20)
set_property(TARGET RayTracer PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

if (MSVC)
    target_compile_options (RayTracer PUBLIC /W3 /WX)

    if (CMAKE_BUILD_TYPE STREQUAL "Release")
		set_property (TARGET RayTracer APPEND_STRING PROPERTY LINK_FLAGS " /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	endif ()
else ()
    target_compile_options (RayTracer PUBLIC -Wall -Wextra -pedantic -Werror -Wno-unused-parameter -Wshadow -Wdouble-promotion)
endif ()

install (FILES
	$<TARGET_FILE:RayTracer>
	DESTINATION .
)
