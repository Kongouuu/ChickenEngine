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
	float2 uv : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = float4(vin.PosL, 1.0f);
	PosW = mul(PosW, gWorldViewProj);
	vout.PosH = mul(PosW,viewProj);
	vout.PosW = PosW;
	vout.Norm = vin.Norm;
	vout.uv = vin.uv;
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 albedo = gDiffuseMap.Sample(gSamLinearWarp, pin.uv);
	// Diffuse
	float3 diffuse = albedo.xyz / PI;

	// Specular
	float3 N = normalize(pin.Norm);
	float3 V = normalize(eyePos - pin.PosW.xyz);
	float NdotV = max(dot(N, V), 0.0f);
	float3 L = normalize(-gDirLight.dir);
	float3 H = normalize(V + L);
	float NdotL = max(dot(N, L), 0.0f);

	float NDF = DistributionGGX(N, H, mRoughness);
	float G = GeometrySmith(N, V, L, mRoughness);

	float3 F0 = mMetallic * albedo.xyz + (1.0f - mMetallic) * float3(0.04f, 0.04f, 0.04f);
	float3 F = fresnelSchlick(F0, V, H);

	float3 numerator = NDF * G * F;
	float denominator = max((4.0f * NdotL * NdotV), 0.001f);
	float3 BRDF = numerator / denominator;
	float3 specular = BRDF;

	// Calculate result
	float3 kS = F;
	float3 kD = float3(1.0, 1.0, 1.0) - kS;
	kD *= (1.0f - mMetallic);

	float3 color = (kD * diffuse +  kS * specular) * gDirLight.strength * NdotL;;
	float3 ambient = albedo.xyz * 0.02;
	color += ambient;
	color = pow(color, (1.0 / 2.2));
	return float4(color,1.0);
}

