#pragma once
#include <stdexcept>

inline std::string HrToString(HRESULT hr)
{
    char s_str[64] = {};
    sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
    return std::string(s_str);
}

class HrException : public std::runtime_error
{
public:
    HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {}
    HRESULT Error() const { return m_hr; }
private:
    const HRESULT m_hr;
};

#define SAFE_RELEASE(p) if (p) (p)->Release()

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw HrException(hr);
    }
}

inline HRESULT ReadDataFromFile(LPCWSTR filename, byte** data, UINT* size)
{
    using namespace Microsoft::WRL;

#if WINVER >= _WIN32_WINNT_WIN8
    CREATEFILE2_EXTENDED_PARAMETERS extendedParams = {};
    extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
    extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
    extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
    extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
    extendedParams.lpSecurityAttributes = nullptr;
    extendedParams.hTemplateFile = nullptr;

    Wrappers::FileHandle file(CreateFile2(filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, &extendedParams));
#else
    Wrappers::FileHandle file(CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN | SECURITY_SQOS_PRESENT | SECURITY_ANONYMOUS, nullptr));
#endif
    if (file.Get() == INVALID_HANDLE_VALUE)
    {
        throw std::exception();
    }

    FILE_STANDARD_INFO fileInfo = {};
    if (!GetFileInformationByHandleEx(file.Get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
    {
        throw std::exception();
    }

    if (fileInfo.EndOfFile.HighPart != 0)
    {
        throw std::exception();
    }

    *data = reinterpret_cast<byte*>(malloc(fileInfo.EndOfFile.LowPart));
    *size = fileInfo.EndOfFile.LowPart;

    if (!ReadFile(file.Get(), *data, fileInfo.EndOfFile.LowPart, nullptr, nullptr))
    {
        throw std::exception();
    }

    return S_OK;
}

inline HRESULT ReadDataFromDDSFile(LPCWSTR filename, byte** data, UINT* offset, UINT* size)
{
    if (FAILED(ReadDataFromFile(filename, data, size)))
    {
        return E_FAIL;
    }

    // DDS files always start with the same magic number.
    static const UINT DDS_MAGIC = 0x20534444;
    UINT magicNumber = *reinterpret_cast<const UINT*>(*data);
    if (magicNumber != DDS_MAGIC)
    {
        return E_FAIL;
    }

    struct DDS_PIXELFORMAT
    {
        UINT size;
        UINT flags;
        UINT fourCC;
        UINT rgbBitCount;
        UINT rBitMask;
        UINT gBitMask;
        UINT bBitMask;
        UINT aBitMask;
    };

    struct DDS_HEADER
    {
        UINT size;
        UINT flags;
        UINT height;
        UINT width;
        UINT pitchOrLinearSize;
        UINT depth;
        UINT mipMapCount;
        UINT reserved1[11];
        DDS_PIXELFORMAT ddsPixelFormat;
        UINT caps;
        UINT caps2;
        UINT caps3;
        UINT caps4;
        UINT reserved2;
    };

    auto ddsHeader = reinterpret_cast<const DDS_HEADER*>(*data + sizeof(UINT));
    if (ddsHeader->size != sizeof(DDS_HEADER) || ddsHeader->ddsPixelFormat.size != sizeof(DDS_PIXELFORMAT))
    {
        return E_FAIL;
    }

    const ptrdiff_t ddsDataOffset = sizeof(UINT) + sizeof(DDS_HEADER);
    *offset = ddsDataOffset;
    *size = *size - ddsDataOffset;

    return S_OK;
}

// Assign a name to the object to aid with debugging.
#if defined(_DEBUG) || defined(DBG)
inline void SetName(ID3D12Object* pObject, LPCWSTR name)
{
    pObject->SetName(name);
}
inline void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index)
{
    WCHAR fullName[50];
    if (swprintf_s(fullName, L"%s[%u]", name, index) > 0)
    {
        pObject->SetName(fullName);
    }
}
#else
inline void SetName(ID3D12Object*, LPCWSTR)
{
}
inline void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT)
{
}
#endif

// Naming helper for ComPtr<T>.
// Assigns the name of the variable as the name of the object.
// The indexed variant will include the index in the name of the object.
#define NAME_D3D12_OBJECT(x) SetName((x).Get(), L#x)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed((x)[n].Get(), L#x, n)

inline UINT CalculateConstantBufferByteSize(UINT byteSize)
{
    // Constant buffer size is required to be aligned.
    return (byteSize + (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);
}

#ifdef D3D_COMPILE_STANDARD_FILE_INCLUDE
inline Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
    const std::wstring& filename,
    const D3D_SHADER_MACRO* defines,
    const std::string& entrypoint,
    const std::string& target)
{
    UINT compileFlags = 0;
#if defined(_DEBUG) || defined(DBG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr;

    Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errors;
    hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

    if (errors != nullptr)
    {
        OutputDebugStringA((char*)errors->GetBufferPointer());
    }
    ThrowIfFailed(hr);

    return byteCode;
}
#endif

// Resets all elements in a ComPtr array.
template<class T>
void ResetComPtrArray(T* comPtrArray)
{
    for (auto &i : *comPtrArray)
    {
        i.Reset();
    }
}


// Resets all elements in a unique_ptr array.
template<class T>
void ResetUniquePtrArray(T* uniquePtrArray)
{
    for (auto &i : *uniquePtrArray)
    {
        i.reset();
    }
}

// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
_Use_decl_annotations_
inline void GetHardwareAdapter(
    IDXGIFactory1* pFactory,
    IDXGIAdapter1** ppAdapter,
    bool requestHighPerformanceAdapter = false)
{
    *ppAdapter = nullptr;

    ComPtr<IDXGIAdapter1> adapter;

    ComPtr<IDXGIFactory6> factory6;
    if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
    {
        for (
            UINT adapterIndex = 0;
            SUCCEEDED(factory6->EnumAdapterByGpuPreference(
                adapterIndex,
                requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
                IID_PPV_ARGS(&adapter)));
            ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    if(adapter.Get() == nullptr)
    {
        for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }
    
    *ppAdapter = adapter.Detach();
}
