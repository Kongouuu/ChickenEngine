#include "Util/Common.hlsl"
#include "Util/RegisterTexture.hlsl"

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
		float4 PosH : SV_POSITION;
		float3 PosL : POSITION;
};


VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Use local vertex position as cubemap lookup vector.
	vout.PosL = vin.PosL;

	// Transform to world space.
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

	// Always center sky about camera.
	posW.xyz += gEyePos;

	// Set z = w so that z/w = 1 (i.e., skydome always on far plane).
	vout.PosH = mul(posW, gViewProj).xyww;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return  gSkyMap.Sample(gSamLinearWrap, pin.PosL);
}
