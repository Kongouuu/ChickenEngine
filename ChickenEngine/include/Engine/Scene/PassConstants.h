#pragma once

#include "Helper/MathHelper.h"
#include "Light.h"
namespace ChickenEngine
{
	struct PassConstants
	{
		DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
		float cbPerObjectPad1 = 0.0f;
		DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
		DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
		float NearZ = 0.0f;
		float FarZ = 0.0f;
		float cbPerObjectPad2 = 0.0f;
		float cbPerObjectPad3 = 0.0f;
		DirectionLight DirLight;
	};

	struct ObjectConstants
	{
		DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
	};

	struct MaterialConstants
	{
		DirectX::XMFLOAT4 Color = { 0.0,0.0,0.0,0.0 };
		float Roughness = 0.0;
		float Metallic = 0.0;
	};
}
