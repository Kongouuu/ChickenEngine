#include "Util/Common.hlsl"
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
	float2 uv : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	// Already in homogeneous clip space.
	vout.PosH = float4(vin.PosL.x, vin.PosL.y, 0.5, 1.0f);

	vout.uv = vin.uv;

	return vout;
}

Texture2D    gDebugMap : register(t1);

float4 PS(VertexOut pin) : SV_Target
{
	return float4(gDebugMap.SampleLevel(gSamLinearWrap, pin.uv, 0).rrr,1.0f);
}


float4 PSShadow(VertexOut pin) : SV_Target
{
	return float4(gDebugMap.SampleLevel(gSamLinearWrap, pin.uv, 0).rrr,1.0f);
}

