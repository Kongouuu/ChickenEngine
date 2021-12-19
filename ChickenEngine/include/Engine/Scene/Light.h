#pragma once
#include "Engine/Core.h"
#include <DirectXMath.h>
namespace ChickenEngine
{
	struct DirectionLight
	{
		DirectX::XMFLOAT3 Strength = { 1.0f,1.0f,1.0f };
		float cbPerObjectPad1 = 0.0f;
		DirectX::XMFLOAT3 Direction = { 1.0f,-1.0f,1.0f };
		float cbPerObjectPad2 = 0.0f;
	};

}
