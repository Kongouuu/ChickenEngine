#pragma once

#include <windowsx.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <wrl.h>

#include "d3dx12.h"
#include "Singleton.h"
#include "MathHelper.h"
#include "FileHelper.h"
#include "DX12Util.h"
#include "Engine/Core.h"
#include "Engine/Log.h"
#include "Renderer/DX12Renderer/CommandList.h"
#include "Renderer/DX12Renderer/Device.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace ChickenEngine
{
	using namespace DirectX;


#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif
}