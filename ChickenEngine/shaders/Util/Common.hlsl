cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
	float4x4 gInvWorld;
	float4 mColor;
	float mRoughness;
	float mMetallic;
};
struct DirLight
{
	float3 strength;
	float cbPerObjectPad1;
	float3 dir;
	float cbPerObjectPad2;
};

cbuffer cbPass : register(b1)
{
	float4x4 gView;
	float4x4 gInvView;
	float4x4 gProj;
	float4x4 gInvProj;
	float4x4 gViewProj;
	float4x4 gInvViewProj;
	float4x4 gShadowTransform;
	float3 gEyePos;
	float cbPerObjectPad1;
	float2 gRenderTargetSize;
	float2 gInvRenderTargetSize;
	float gNearZ;
	float gFarZ;
	float gVsmMinVar; // tmp, shoule be cbPerObjectPad2
	float gVsmAmount;
	DirLight gDirLight;
};

cbuffer cbSetting : register(b2)
{
	int gShadowType;
	bool gEnableShadow;
};

Texture3D    gSkyMap : register(t0);
Texture2D    gShadowMap : register(t1);
Texture2D    gDiffuseMap : register(t2);


SamplerState gSamPointWrap : register(s0);
SamplerState gSamPointClamp : register(s1);
SamplerState gSamLinearWrap : register(s2);
SamplerState gSamLinearClamp : register(s3);
SamplerState gSamAnisotropicWrap : register(s4);
SamplerState gSamAnisotropicClamp : register(s5);
SamplerComparisonState gSamShadow : register(s6);

