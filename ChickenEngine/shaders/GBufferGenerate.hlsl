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
	vout.PosW.w = vout.PosH.w;

	vout.Norm = mul(transpose((float3x3)gInvWorld), vin.Norm);
	vout.uv = vin.uv;
	return vout;
}

PixelOut PS(VertexOut pin) : SV_Target
{
	PixelOut pout;
	float4 albedo = gDiffuseMap.Sample(gSamLinearWrap, pin.uv) + mColor;
	float3 normal = normalize(pin.Norm); //NDCToTexture(normalize(pin.Norm));
	// lerp

	pout.gbuffer0 = float4(albedo.xyz, mRoughness);
	pout.gbuffer1 = float4(pin.PosW.xyzw);
	pout.gbuffer2 = float4(normal, mMetallic);
	return pout;
}
