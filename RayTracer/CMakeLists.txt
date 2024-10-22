cmake_minimum_required (VERSION 3.16)

set (VENDOR_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Vendor")

if (MSVC)
    list (APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/Vendor)
    set (SFML_STATIC_LIBRARIES TRUE)
endif ()

find_program (CPPCHECK_BIN NAMES cppcheck)
if (CPPCHECK_BIN)
	set (CMAKE_CXX_CPPCHECK ${CPPCHECK_BIN})

    list (APPEND CMAKE_CXX_CPPCHECK
        "--enable=warning"
        "--force"
        "--inline-suppr"
        "--quiet"
        "--std=c++20"
    )

    message ("Using cppcheck configuration: ${CMAKE_CXX_CPPCHECK}")
endif ()

find_package (SFML CONFIG REQUIRED COMPONENTS graphics)
find_package (fkYAML CONFIG REQUIRED HINTS ${CMAKE_SOURCE_DIR}/Vendor/fkyaml-0.3.13/package/share/cmake/fkYAML)

add_executable (RayTracer
	"Engine/Renderer.cpp"
    "Engine/Camera.cpp"
    "Engine/Color.cpp"
    "Engine/Mesh.cpp"
    "Engine/Material.cpp"
    "Engine/Material/DebugMaterial.cpp"
    "Engine/Material/DielectricMaterial.cpp"
    "Engine/Material/DiffuseMaterial.cpp"
    "Engine/Material/LightMaterial.cpp"
    "Engine/Material/ReflectiveMaterial.cpp"
    "Engine/Object.cpp"
    "Engine/Object/BoxObject.cpp"
    "Engine/Object/MeshObject.cpp"
    "Engine/Object/PlaneObject.cpp"
    "Engine/Object/SphereObject.cpp"
    "Engine/Texture.cpp"
    "Engine/Texture/CheckerboardTexture.cpp"
    "Engine/Texture/ImageTexture.cpp"
    "Engine/Texture/SolidTexture.cpp"
    "Engine/Ray.cpp"
    "Engine/Vector.cpp"
    "Main.cpp"
    "SceneLoader.cpp"
    "Viewer.cpp"
    "$<$<BOOL:WINDOWS>:WindowsResources.rc>"
)

target_include_directories (RayTracer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (RayTracer PRIVATE
	sfml-graphics
	fkYAML::fkYAML
	objl::objl
)
target_compile_features (RayTracer PUBLIC cxx_std_20)

set_property(TARGET RayTracer PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

if (MSVC)
    target_compile_options (RayTracer PUBLIC /W4 /WX /wd4100 /arch:AVX2 /Zi)
    target_compile_definitions (RayTracer PUBLIC _CRT_SECURE_NO_WARNINGS)
    target_link_options(RayTracer PUBLIC /DEBUG)
else ()
    target_compile_options (RayTracer PUBLIC -Wall -Wextra -pedantic -Werror -Wno-unused-parameter -Wshadow -Wdouble-promotion -g)
endif ()

add_custom_command (TARGET RayTracer POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Assets" "$<TARGET_FILE_DIR:RayTracer>/Assets"
	COMMENT "Copying original assets...")

add_custom_command (TARGET RayTracer POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy_directory "${VENDOR_BASE_DIR}/PublicDomainTextures.com/Assets" "$<TARGET_FILE_DIR:RayTracer>/Assets"
	COMMENT "Copying 'PublicDomainTextures.com' third party assets...")

add_custom_command (TARGET RayTracer POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy_directory "${VENDOR_BASE_DIR}/cs.utah.edu/Assets" "$<TARGET_FILE_DIR:RayTracer>/Assets"
	COMMENT "Copying 'cs.utah.edu' third party assets...")

add_custom_command (TARGET RayTracer POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy_directory "${VENDOR_BASE_DIR}/thebasemesh.com/Assets" "$<TARGET_FILE_DIR:RayTracer>/Assets"
	COMMENT "Copying 'thebasemesh.com' third party assets...")

add_custom_command (TARGET RayTracer POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy_directory "${VENDOR_BASE_DIR}/openfontlibrary.org/Assets" "$<TARGET_FILE_DIR:RayTracer>/Assets"
	COMMENT "Copying 'openfontlibrary.org' third party assets...")

add_custom_command (TARGET RayTracer POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy_directory "${VENDOR_BASE_DIR}/Blender/Assets" "$<TARGET_FILE_DIR:RayTracer>/Assets"
	COMMENT "Copying 'Blender' third party assets...")

add_custom_command (TARGET RayTracer POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy_directory "${VENDOR_BASE_DIR}/astrellon/Assets" "$<TARGET_FILE_DIR:RayTracer>/Assets"
	COMMENT "Copying 'astrellon' third party assets...")

add_custom_command (TARGET RayTracer POST_BUILD
	COMMAND ${CMAKE_COMMAND}
	ARGS -E copy_directory "${VENDOR_BASE_DIR}/uvchecker.vinzi.xyz/Assets" "$<TARGET_FILE_DIR:RayTracer>/Assets"
	COMMENT "Copying 'uvchecker.vinzi.xyz' third party assets...")

install (FILES
	$<TARGET_FILE:RayTracer>
	$<TARGET_FILE_DIR:RayTracer>/Assets
	DESTINATION .
)
