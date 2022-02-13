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
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = float4(vin.PosL, 1.0f);
	PosW = mul(PosW, gWorld);
	vout.PosH = mul(PosW, gShadowTransform);
	return vout;
}

float PS(VertexOut pin) : SV_Target
{
	return float(pin.PosH.z * pin.PosH.z);
}
