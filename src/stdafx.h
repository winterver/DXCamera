#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>

#include "D3DX12/d3dx12.h"              // Shit design: some versions of WinSDK do not provide this, you need to download this from the offical github repo
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

#include "DXSample/DXHelper.h"
