#pragma once
#include "d3dx12.h"

#include <windowsx.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <wrl.h>

#include "Engine/Log.h"

namespace ChickenEngine
{
	enum EVertexLayout
	{
		POS_NORM_COL = 0,
		POS_NORM_TEX,
		POS_NORM_TAN_TEX,
		NONE
	};
}
