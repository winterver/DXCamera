cmake_minimum_required(VERSION 3.27)
project(DXCamera)

add_definitions(/D_DEBUG /D_UNICODE)
add_executable(
    DXCamera
    src/Main.cpp
    src/D3D12HelloTriangle.cpp
    src/DXSample/DXSample.cpp
    src/DXSample/Win32Application.cpp)
target_precompile_headers(DXCamera PRIVATE src/stdafx.h)
target_link_options(DXCamera PRIVATE /subsystem:windows)
target_link_libraries(DXCamera d3d12 dxgi D3DCompiler)
