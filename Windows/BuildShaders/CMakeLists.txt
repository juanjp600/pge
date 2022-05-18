
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

target_link_libraries(BuildShaders PUBLIC
    # ${SDL2_LIBRARIES}
    Engine
    d3d11
    dxgi
    dxguid
    )

target_include_directories(BuildShaders PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Include
    # ${CMAKE_CURRENT_SOURCE_DIR}/../../Libraries/DirectXShaderCompiler/include
    # ${CMAKE_CURRENT_SOURCE_DIR}/../../Libraries/SPIRV-Cross
    # ${CMAKE_CURRENT_SOURCE_DIR}/../../Libraries/ShaderConductor/include
    )

add_custom_command(TARGET BuildShaders POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:BuildShaders> $<TARGET_FILE_DIR:BuildShaders>
    COMMAND_EXPAND_LISTS
    )
