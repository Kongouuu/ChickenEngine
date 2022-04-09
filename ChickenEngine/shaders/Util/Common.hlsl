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

struct PointLight
{
	float3 position;
	float cpPad;
	float3 strength;
	int attenIndex;
};

/*
Attenuation List:
| Index | dist | linear | quadratic
   11	  3250   0.0014   0.000007
   10     600    0.007    0.0002
   9      325    0.014    0.0007
   8      200    0.022    0.0019
   7      160    0.027    0.0028
   6      100    0.045    0.0075
   5      65     0.07     0.017
   4      50     0.09     0.032
   3      32     0.14     0.07
   2      20     0.22     0.20
   1      13     0.35     0.44
   0      7      0.7      1.8
*/

static const float2 attenuationCoeff[12] = {
	float2(0.7,1.8),
	float2(0.35, 0.44),
	float2(0.22,0.20),
	float2(0.14,0.07),
	float2(0.09,0.032),
	float2(0.07,0.017),
	float2(0.045,0.0075),
	float2(0.027, 0.0028),
	float2(0.022, 0.0019),
	float2(0.014, 0.0007),
	float2(0.007, 0.0002),
	float2(0.0014, 0.00007)
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
	PointLight gPointLight[16];
	int gNumPointLight;
};

cbuffer cbSetting : register(b2)
{
	int gShadowType;
	bool gSSAOEnabled;
};

SamplerState gSamPointWrap : register(s0);
SamplerState gSamPointClamp : register(s1);
SamplerState gSamLinearWrap : register(s2);
SamplerState gSamLinearClamp : register(s3);
SamplerState gSamAnisotropicWrap : register(s4);
SamplerState gSamAnisotropicClamp : register(s5);
SamplerComparisonState gSamShadow : register(s6);

