#pragma once

#include "Helper/MathHelper.h"
namespace ChickenEngine
{
	struct Vertex
	{
		Vertex() {}

		Vertex(
			DirectX::XMFLOAT3 p,
			DirectX::XMFLOAT3 n,
			DirectX::XMFLOAT3 t,
			DirectX::XMFLOAT3 b,
			DirectX::XMFLOAT2 t1) :
			pos(p), normal(n), tangent(t), bitangent(b), texC(t1) {}


		DirectX::XMFLOAT3 pos = { 0.0,0.0,0.0 };
		DirectX::XMFLOAT3 normal = { 0.0,0.0,0.0 };
		DirectX::XMFLOAT3 tangent = { 0.0,0.0,0.0 };
		DirectX::XMFLOAT3 bitangent = { 0.0,0.0,0.0 };
		DirectX::XMFLOAT2 texC = { 0.0,0.0 };
	};

	struct DirectionLightData
	{
		DirectX::XMFLOAT3 Strength = { 1.0f,1.0f,1.0f };
		float cbPerObjectPad1 = 0.0f;
		DirectX::XMFLOAT3 Direction = { 0.0f,-0.99f,0.0f };
		float cbPerObjectPad2 = 0.0f;
	};

	struct DirectionLight
	{
		DirectionLightData data;
		DirectX::XMFLOAT3 Position = { 0.0f, 50.0f, 0.0f };
		DirectX::XMFLOAT3 Rotation = { 0.0f,0.0f,10.0f };
	};

	struct PassConstants
	{
		DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 ShadowTransform = MathHelper::Identity4x4();
		DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
		float cbPerObjectPad1 = 0.0f;
		DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
		DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
		float NearZ = 0.0f;
		float FarZ = 0.0f;
		float cbPerObjectPad2 = 0.0f;
		float cbPerObjectPad3 = 0.0f;
		DirectionLightData DirLight;
	};

	struct ObjectConstants
	{
		DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvWorld = MathHelper::Identity4x4();
		DirectX::XMFLOAT4 Color = { 0.0,0.0,0.0,0.0 };
		float Roughness = 0.0;
		float Metallic = 0.0;
	};

	enum ETextureDimension
	{
		TEXTURE2D = 0,
		TEXTURE3D
	};

	enum ETextureType
	{
		DIFFUSE = 0,
		SPECULAR,
		NORMAL,
		HEIGHT,
		NULL_TYPE
	};
}