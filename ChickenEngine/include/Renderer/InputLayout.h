#pragma once

#include "pch.h"
#include "Helper/DX12CommonHeader.h"
#include "Engine/Core.h"

namespace ChickenEngine
{
	class CHICKEN_API InputLayout
	{
	public:
		static std::vector<D3D12_INPUT_ELEMENT_DESC> GetInputLayout();
	};
}