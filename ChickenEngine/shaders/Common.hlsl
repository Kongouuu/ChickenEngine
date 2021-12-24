cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
};

cbuffer cbMaterial : register(b1)
{
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

cbuffer cbPass : register(b2)
{
	float4x4 view;
	float4x4 invView;
	float4x4 proj;
	float4x4 invProj;
	float4x4 viewProj;
	float4x4 invViewProj;
	float3 eyePos;
	float cbPerObjectPad1;
	float2 gRenderTargetSize;
	float2 gInvRenderTargetSize;
	float gNearZ;
	float gFarZ;
	float cbPerObjectPad2;
	float cbPerObjectPad3;
	DirLight gDirLight;
};


Texture2D    gDiffuseMap : register(t2);
SamplerState gSamPointWrap : register(s0);
SamplerState gSamPointClamp : register(s1);
SamplerState gSamLinearWarp : register(s2);
SamplerState gSamLinearClamp : register(s3);
SamplerState gSamAnisotropicWarp : register(s4);
SamplerState gSamAnisotropicClamp : register(s5);

