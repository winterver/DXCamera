#pragma once

class DXSample
{
public:
    DXSample(UINT width, UINT height, std::wstring name);
    virtual ~DXSample();

    virtual void OnInit() = 0;
    virtual void OnUpdate() = 0;
    virtual void OnRender() = 0;
    virtual void OnDestroy() = 0;

    virtual void OnKeyDown(UINT8 key)   { }
    virtual void OnKeyUp(UINT8 key)     { }

    UINT GetWidth() const           { return m_width; }
    UINT GetHeight() const          { return m_height; }
    const WCHAR* GetTitle() const   { return m_title.c_str(); }
    void SetHWND(HWND hWnd)         { m_hWnd = hWnd; }

protected:
    UINT m_width;
    UINT m_height;
    float m_aspectRatio;
    HWND m_hWnd;

private:
    std::wstring m_title;
};
