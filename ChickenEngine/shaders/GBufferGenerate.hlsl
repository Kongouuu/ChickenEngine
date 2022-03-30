#include "Util/Common.hlsl"
#include "Util/RegisterTexture.hlsl"
#include "Util/Functions.hlsl"
#include "Util/ShadowUtil.hlsl"


struct VertexIn
{
	float3 PosL  : POSITION;
	float3 Norm : NORMAL;
	float3 Tan : TANGENT;
	float3 BiTan : BITANGENT;
	float2 uv : TEXCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float4 ShadowPosH : POSITION_SHADOW;
	float4 PosW  : POSITION_WORLD;
	float3 Norm  : NORMAL;
	float2 uv : TEXCOORD;
};

struct PixelOut {
	// albedo roughness
	float4 gbuffer0 : SV_Target0;
	// word position, w is depth
	float4 gbuffer1 : SV_Target1;
	// normal  metallic
	float4 gbuffer2 : SV_Target2;
};


VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = float4(vin.PosL, 1.0f);
	PosW = mul(PosW, gWorld);
	vout.PosH = mul(PosW, gViewProj);
	vout.PosW = PosW;
	vout.ShadowPosH = mul(PosW, gShadowTransform);
	vout.ShadowPosH /= vout.ShadowPosH.w;
	// from ndc to texture 
	float4x4 T = float4x4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	vout.ShadowPosH = mul(vout.ShadowPosH, T);
	vout.Norm = mul(transpose((float3x3)gInvWorld), vin.Norm);
	vout.uv = vin.uv;
	return vout;
}

PixelOut PS(VertexOut pin) : SV_Target
{
	PixelOut pout;
	float4 albedo = gDiffuseMap.Sample(gSamLinearWrap, pin.uv) + mColor;
	float3 normal = NDCToTexture(normalize(pin.Norm));
	float shadowFactor = 1.0f;
	// lerp
	shadowFactor = CalcShadowFactor(pin.ShadowPosH);

	pout.gbuffer0 = float4(albedo.xyz * shadowFactor, mRoughness);
	pout.gbuffer1 = float4(pin.PosW.xyz, pin.PosH.z);
	pout.gbuffer2 = float4(normal, mMetallic);
	return pout;
}
