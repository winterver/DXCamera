#include "stdafx.h"
#include "DXSample/Win32Application.h"
#include "D3D12HelloTriangle.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    SetProcessDPIAware(); // Fuck Windows' DPI adjustment off
    D3D12HelloTriangle sample(1600, 900, L"D3D12 Hello Triangle");
    return Win32Application::Run(&sample, hInstance, nCmdShow);
}
