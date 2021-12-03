#pragma once
#include "d3dx12.h"

#include <windowsx.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <wrl.h>

#include "Engine/Log.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

#define CONCAT2(X, Y) X##Y
#define CONCAT(X, Y) CONCAT2(X, Y)
#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)
#define WIDEN(X) CONCAT(L, STRINGIFY(X))

namespace ChickenEngine
{


	enum EVertexLayout
	{
		POS_NORM_COL = 0,
		POS_NORM_TEX,
		POS_NORM_TAN_TEX,
		NONE
	};




	inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw std::exception();
    }
}
}
