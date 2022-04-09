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
		DirectX::XMFLOAT3 strength = { 1.0f,1.0f,1.0f };
		float cbPerObjectPad1 = 0.0f;
		DirectX::XMFLOAT3 direction = { 0.0f,-0.99f,0.0f };
		float cbPerObjectPad2 = 0.0f;
	};

	struct DirectionLight
	{
		DirectionLightData data;
		DirectX::XMFLOAT3 position = { 0.0f, 50.0f, 0.0f };
		DirectX::XMFLOAT3 rotation = { 0.0f,0.0f,10.0f };
		bool bAutoPosition = true;
		float distFrustumCenter = 0.0;
		float offsetViewDir = 0.0;
	};

	struct PointLight
	{
		DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
		float cbPad;
		DirectX::XMFLOAT3 strength = { 0.0f, 0.0f, 0.0f };
		int attenIndex = 1;
	};
	
	static std::unordered_map<int, int> attenMap({
		{ 7, 0 },
		{ 13, 1 },
		{ 20, 2 },
		{ 32, 3 },
		{ 50, 4 },
		{ 65, 5 },
		{ 100, 6 },
		{ 160, 7 },
		{ 200, 8 },
		{ 325, 9 },
		{ 600, 10 },
		{ 3250, 11 },
	});

	inline int AttenuationDistToIndex(int dist)
	{
		if (attenMap.find(dist) != attenMap.end())
			return attenMap[dist];
		return 11;
	}

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
		float vsmMinVariance = 0.0f;
		float vsmAmount = 0.0f;
		DirectionLightData DirLight;
		PointLight PointLights[16];
		int NumPointLight = 0;
	};

	struct ObjectConstants
	{
		DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvWorld = MathHelper::Identity4x4();
		DirectX::XMFLOAT4 Color = { 0.0,0.0,0.0,0.0 };
		float Roughness = 0.0;
		float Metallic = 0.0;
	};

	enum EShadowType
	{
		SM_DISABLED = 0,
		SM_DEFAULT,
		SM_PCF,
		SM_PCSS,
		SM_VSSM
	};

	struct RenderBranch
	{
		int sm_type;
		bool ssao_enable;
	};

	struct RenderSettings
	{
		RenderBranch branch;
		bool bGenerateSM;
		bool bGenerateGBuffer;
		bool bDeferredPass;
		//bool sm_generateVSM;
		
	};

	enum ETextureDimension
	{
		TEXTURE2D = 0,
		TEXTURE3D
	};

	enum ETextureType
	{
		DIFFUSE = 0,
		NORMAL,
		HEIGHT,
		NULL_TYPE
	};

	enum ERenderLayer
	{
		L_SKYBOX = 0,
		L_BACKGROUND,
		L_DEFAULT,
		L_TRANSPARENT,
		L_OVERLAY,
		L_COUNT
	};
}