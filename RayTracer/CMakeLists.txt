cmake_minimum_required (VERSION 3.16)

set (VENDOR_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor")

if (MSVC)
    list (APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/Vendor)
    set(SFML_STATIC_LIBRARIES TRUE)
endif ()

find_package (SFML CONFIG REQUIRED COMPONENTS graphics)

add_executable (RayTracer
	"Engine/Renderer.cpp"
    "Engine/Camera.cpp"
    "Engine/Light.cpp"
    "Engine/Mesh.cpp"
    "Engine/Object.cpp"
    "Engine/Object/BoxObject.cpp"
    "Engine/Object/MeshObject.cpp"
    "Engine/Object/PlaneObject.cpp"
    "Engine/Object/SphereObject.cpp"
    "Engine/Texture/CheckerboardTexture.cpp"
    "Engine/Texture/ImageTexture.cpp"
    "Engine/Texture/SolidTexture.cpp"
    "Engine/Ray.cpp"
    "ExampleScene.cpp"
    "Main.cpp"
)

target_include_directories (RayTracer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/OBJ-Loader/Source)
target_link_libraries (RayTracer PRIVATE sfml-graphics)
target_compile_features (RayTracer PUBLIC cxx_std_20)

set_property(TARGET RayTracer PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

if (MSVC)
    target_compile_options (RayTracer PUBLIC /W4 /WX /wd4100)
else ()
    target_compile_options (RayTracer PUBLIC -Wall -Wextra -pedantic -Werror -Wno-unused-parameter -Wshadow -Wdouble-promotion)
endif ()

add_custom_command (TARGET RayTracer POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Assets" "$<TARGET_FILE_DIR:RayTracer>/Assets"
	COMMENT "Copying original assets...")

add_custom_command (TARGET RayTracer POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/PublicDomainTextures.com/Assets" "$<TARGET_FILE_DIR:RayTracer>/Assets"
	COMMENT "Copying 'PublicDomainTextures.com' third party assets...")

add_custom_command (TARGET RayTracer POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/cs.utah.edu/Assets" "$<TARGET_FILE_DIR:RayTracer>/Assets"
	COMMENT "Copying 'cs.utah.edu' third party assets...")

add_custom_command (TARGET RayTracer POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor/thebasemesh.com/Assets" "$<TARGET_FILE_DIR:RayTracer>/Assets"
	COMMENT "Copying 'thebasemesh.com' third party assets...")

install (FILES
	$<TARGET_FILE:RayTracer>
	DESTINATION .
)
