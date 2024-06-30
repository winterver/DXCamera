cmake_minimum_required(VERSION 3.27)
project(DXCamera)

add_definitions(/D_UNICODE /DWIN32_LEAN_AND_MEAN)
add_executable(
    DXCamera
    src/Main.cpp
    src/DXCamera.cpp
    src/Win32Application.cpp)
target_link_options(DXCamera PRIVATE /subsystem:windows)
target_link_libraries(DXCamera d3d12 dxgi D3DCompiler)
