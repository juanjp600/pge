project(Engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_subdirectory(Libraries/SDL2)
add_subdirectory(Windows/BuildShaders)
# add_subdirectory(Libraries/SPIRV-Reflect)

file(GLOB_RECURSE SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/glad/src/gl.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/SPIRV-Reflect/spirv_reflect.c"

    )
list(FILTER SOURCE_FILES EXCLUDE REGEX "Init.cpp")

if(NOT WIN32 OR NOT D3D)
    list(FILTER SOURCE_FILES EXCLUDE REGEX "DX11")
endif()

# add_compile_definitions(SDL_MAIN_HANDLED)
add_compile_definitions(NOMINMAX)
# add_compile_definitions(VULKAN_HPP_DISABLE_ENHANCED_MODE)

add_library(Engine STATIC)
target_sources(Engine PUBLIC "${SOURCE_FILES}")

target_link_libraries(Engine PUBLIC SDL2)
if(WIN32 AND D3D)
    target_link_libraries(Engine PUBLIC d3d11 dxgi dxguid)
endif()
if(VKR)
    target_link_libraries(Engine PUBLIC Vulkan::Vulkan)
    target_include_directories(Engine PUBLIC Vulkan::Headers)
endif()

target_include_directories(Engine
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/Include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/glad/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/SDL2/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/SPIRV-Reflect"
    "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/SPIRV-Reflect/include"
    )

# GIT_URL
execute_process(
    COMMAND git config --get remote.origin.url
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_URL
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
# GIT_BRANCH
execute_process(
    COMMAND git branch --show-current
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
# GIT_COMMIT
execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Src/Info/Info.cpp.in ${CMAKE_CURRENT_SOURCE_DIR}/Src/Info/Info.cpp @ONLY)
