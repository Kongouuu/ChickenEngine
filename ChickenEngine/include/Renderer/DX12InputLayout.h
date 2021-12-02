#pragma once

#include "pch.h"
#include "DX12CommonHeader.h"
#include "Engine/Core.h"

namespace ChickenEngine
{
	class CHICKEN_API DX12InputLayout
	{
	public:
		static const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputLayout(EVertexLayout layoutType, bool& result);
	};
}