set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(BuildShaders
    ${CMAKE_CURRENT_SOURCE_DIR}/BuildShaders.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CompileResult.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Glsl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Parser.cpp
    )

target_link_libraries(BuildShaders PUBLIC Engine)
if(WIN32 AND PGE_D3D)
    target_link_libraries(BuildShaders PUBLIC d3d11 dxgi dxguid d3dcompiler)
endif()

target_include_directories(BuildShaders PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Include
    )

add_custom_command(TARGET BuildShaders POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:BuildShaders> $<TARGET_FILE_DIR:BuildShaders>
    COMMAND_EXPAND_LISTS
    )
