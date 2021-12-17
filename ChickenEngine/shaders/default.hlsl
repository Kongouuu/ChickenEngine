#include "Common.hlsl"
#include "ShadingUtil.hlsl"

struct VertexIn
{
	float3 PosL  : POSITION;
	float3 Norm : NORMAL;
	float2 uv : TEXCOORD;
};

struct VertexOut
{ 
	float4 PosH  : SV_POSITION;
	float4 PosW  : POSITION_WORLD;
	float3 Norm  : NORMAL;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = float4(vin.PosL, 1.0f);
	PosW = mul(PosW, gWorldViewProj);
	vout.PosH = mul(PosW,viewProj);
	vout.PosW = PosW;
	vout.Norm = vin.Norm;
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	// Diffuse
	float4 diffuse = mColor / PI;

	// Specular
	float3 N = normalize(pin.Norm);
	float3 V = normalize(eyePos - pin.PosW.xyz);
	float NdotV = max(dot(N, V), 0.0);
	float3 L = normalize(-gDirLight.dir);
	float3 H = normalize(V + L);
	float NdotL = max(dot(N, L), 0.0);

	float NDF = DistributionGGX(N, H, mRoughness);
	float G = GeometrySmith(N, V, L, mRoughness);
	float F = fresnelSchlick(mMetallic, V, H);

	float numerator = NDF * G * F;
	float denominator = max((4.0f * NdotL * NdotV), 0.001);
	float BRDF = numerator / denominator;

	float4 specular = float4(BRDF * mColor.xyz * NdotL , 1.0f);

	float4 color = diffuse + specular;
	color = pow(color, (1.0 / 2.2));
	return color;
}

