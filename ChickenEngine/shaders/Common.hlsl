cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
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
	float cbPerObjectPad2;
	float cbPerObjectPad3;
	DirLight gDirLight;
};

Texture3D    gSkyMap : register(t0);
Texture2D    gShadowMap : register(t1);
Texture2D    gDiffuseMap : register(t2);
SamplerState gSamPointWrap : register(s0);
SamplerState gSamPointClamp : register(s1);
SamplerState gSamLinearWarp : register(s2);
SamplerState gSamLinearClamp : register(s3);
SamplerState gSamAnisotropicWarp : register(s4);
SamplerState gSamAnisotropicClamp : register(s5);
SamplerComparisonState gSamShadow : register(s6);

float CalcShadowFactor(float4 shadowPosH)
{
	// Complete projection by doing division by w.
	//shadowPosH.xyz /= shadowPosH.w;

	// Depth in NDC space.
	float depth = shadowPosH.z;

	uint width, height, numMips;
	gShadowMap.GetDimensions(0, width, height, numMips);

	// Texel size.
	float dx = 1.0f / (float)width;

	float percentLit = 0.0f;
	const float2 offsets[9] =
	{
		float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
	};

	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		percentLit += gShadowMap.SampleCmpLevelZero(gSamShadow,
			shadowPosH.xy + offsets[i], depth).r;
	}

	return percentLit / 9.0f;
}