#include "Win32Application.h"

LRESULT CALLBACK Win32Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Win32Application* pSample = (Win32Application*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    switch (message)
    {
    case WM_CREATE: {
            LPCREATESTRUCT pCreateStruct = (LPCREATESTRUCT)lParam;
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pCreateStruct->lpCreateParams);
        }
        return 0;

    case WM_PAINT:
        /* If Begin/EndPaint is not called within WM_PAINT,
        *  Windows will constantly repost WM_PAINT. That's
        *  right what we want for a DirectX application.
        *  And that is exactly how the offical DX12 Samples
        *  keep the client area being refreshed. */
        //PAINTSTRUCT paint;
        //HDC hdc = BeginPaint(hWnd, &paint);
        //EndPaint(hWnd, &paint);

        if (pSample) {
            pSample->OnUpdate();
            pSample->OnRender();
        }
        return 0;

    case WM_KEYDOWN:
        if (pSample) {
            pSample->OnKeyDown((UINT8)wParam);
        }
        return 0;

    case WM_KEYUP:
        if (pSample) {
            pSample->OnKeyUp((UINT8)wParam);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

int Win32Application::Run()
{
    WNDCLASSEX windowClass = { 0 };
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = GetModuleHandle(NULL);
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = L"DXCameraClass";
    RegisterClassEx(&windowClass);

    UINT32 windowFlags = WS_OVERLAPPEDWINDOW;
    RECT windowRect = { 0, 0, m_initialWidth, m_initialHeight };
    //AdjustWindowRect(&windowRect, windowFlags, FALSE);

    m_hWnd = CreateWindow(
        windowClass.lpszClassName,
        m_initialTitle.c_str(),
        windowFlags,
        (GetSystemMetrics(SM_CXSCREEN) - (windowRect.right - windowRect.left))/2,
        (GetSystemMetrics(SM_CYSCREEN) - (windowRect.bottom - windowRect.top))/2,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        windowClass.hInstance,
        this);

    OnInit();
    ShowWindow(m_hWnd, SW_SHOW);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    OnDestroy();
    return (char)msg.wParam;
}
