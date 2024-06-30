#include "DXCamera.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    SetProcessDPIAware();
    DXCamera camera(L"DXCamera", 1600, 900);
    return camera.Run();
}
