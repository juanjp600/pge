add_executable(BuildShaders
    ${CMAKE_CURRENT_SOURCE_DIR}/BuildShaders.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CompileResult.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Glsl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Parser.cpp
    )

target_link_libraries(BuildShaders PUBLIC
    ${SDL2_LIBRARIES}
    Engine
    d3d11
    dxgi
    dxguid
    )

target_include_directories(BuildShaders PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    )
