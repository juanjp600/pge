project(Engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_subdirectory(Libraries/SDL2)
add_subdirectory(Windows/BuildShaders)

file(GLOB_RECURSE GFX_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Graphics/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Graphics/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/ResourceManagement/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/ResourceManagement/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/glad/src/gl.c
    )

if(NOT WIN32 OR NOT PGE_D3D)
    list(FILTER GFX_SOURCE_FILES EXCLUDE REGEX "DX11")
endif()

if(NOT PGE_OGL)
    list(FILTER GFX_SOURCE_FILES EXCLUDE REGEX "OGL3")
    list(FILTER GFX_SOURCE_FILES EXCLUDE REGEX "gl.c")
endif()

add_compile_definitions(SDL_MAIN_HANDLED)
add_compile_definitions(NOMINMAX)

add_library(Engine STATIC)
target_compile_definitions(Engine PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")

# Make sure to remove Info.cpp.in and Init.cpp when making this list!
# All files in the Graphics and ResourceManagement folders are auto added.
target_sources(Engine PUBLIC
    ${GFX_SOURCE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Color/ConsoleColor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Exception/Exception.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/File/BinaryDefaultSpecializations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/File/BinaryReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/File/BinaryWriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/File/FilePath.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/File/TextReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/File/TextWriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Info/Info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Input/Input.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Input/InputManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Input/InputManagerInternal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Math/Assertions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Math/Random.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/String/String.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/String/Unicode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/String/UnicodeHelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/String/UnicodeHelper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/String/UnicodeInternal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/String/UnicodeManual.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/StructuredData/StructuredData.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/SysEvents/SysEvents.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/SysEvents/SysEventsInternal.h
    )

target_link_libraries(Engine PUBLIC SDL2)
if(WIN32 AND PGE_D3D)
    target_link_libraries(Engine PUBLIC d3d11 dxgi dxguid)
endif()
if(PGE_VKR)
    target_link_libraries(Engine PUBLIC Vulkan::Vulkan)
    target_include_directories(Engine PUBLIC Vulkan::Headers)
endif()

target_include_directories(Engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/SDL2/include
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
