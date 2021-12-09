#pragma once

#include <windowsx.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXColors.h>
#include <wrl.h>

#include "d3dx12.h"
#include "Singleton.h"
#include "MathHelper.h"
#include "DX12Defines.h"
#include "Engine/Core.h"
#include "Engine/Log.h"
#include "Renderer/CommandList.h"
#include "Renderer/Device.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

