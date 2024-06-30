#pragma once
#include <windows.h>
#include <string>

class Win32Application
{
public:
    Win32Application(std::wstring title, int width, int height)
        : m_initialTitle(title)
        , m_initialWidth(width)
        , m_initialHeight(height)       { }
    virtual ~Win32Application()         { }

    int GetClientWidth()                { RECT rect; GetClientRect(m_hWnd, &rect); return rect.right - rect.left; }
    int GetClientHeight()               { RECT rect; GetClientRect(m_hWnd, &rect); return rect.bottom - rect.top; }
    float GetAspectRatio()              { return float(GetClientWidth())/GetClientHeight(); }
    HWND GetHWND()                      { return m_hWnd; }
    int Run();

protected:
    virtual void OnInit()               = 0;
    virtual void OnUpdate()             = 0;
    virtual void OnRender()             = 0;
    virtual void OnDestroy()            = 0;

    virtual void OnKeyDown(UINT8 key)   { }
    virtual void OnKeyUp(UINT8 key)     { }

private:
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    std::wstring m_initialTitle;
    int m_initialWidth;
    int m_initialHeight;
    HWND m_hWnd;
};
