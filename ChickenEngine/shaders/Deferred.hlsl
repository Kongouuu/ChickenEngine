#include "Util/Common.hlsl"
#include "Util/RegisterTextureDeferred.hlsl"
#include "Util/BRDF.hlsl"
#include "Util/Functions.hlsl"
#include "Util/ShadowUtil.hlsl"

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float2 UV : TEXCOORD;
};

static const float2 gTexCoords[6] =
{
	float2(0.0f, 1.0f),
	float2(0.0f, 0.0f),
	float2(1.0f, 0.0f),
	float2(0.0f, 1.0f),
	float2(1.0f, 0.0f),
	float2(1.0f, 1.0f)
};


VertexOut VS(uint vid : SV_VertexID)
{
	VertexOut vout;

	vout.UV = gTexCoords[vid];

	// Quad covering screen in NDC space.
	vout.PosH = float4(2.0f * vout.UV.x - 1.0f, 1.0f - 2.0f * vout.UV.y, 0.0f, 1.0f);

	return vout;
}


float4 PS(VertexOut pin) : SV_Target
{
	float4 albedo = gAlbedoMap.Sample(gSamLinearWrap, pin.UV);
	float4 position = gPositionMap.Sample(gSamLinearWrap, pin.UV);
	float4 normal = gNormalMap.Sample(gSamLinearWrap, pin.UV);
	float roughness = albedo.w;
	float metallic = normal.w;
	[branch]
	if (metallic > 0.99f)
		clip(-1);
	// Diffuse
	float3 diffuse = albedo.xyz / PI;

	// Specular
	float3 N = normalize(normal);
	float3 V = normalize(gEyePos - position.xyz);
	float NdotV = max(dot(N, V), 0.0f);
	float3 L = normalize(-gDirLight.dir);
	float3 H = normalize(V + L);
	float NdotL = max(dot(N, L), 0.0f);

	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);

	float3 F0 = metallic * albedo.xyz + (1.0f - metallic) * float3(0.04f, 0.04f, 0.04f);
	float3 F = FresnelSchlick(F0, V, H);

	float3 numerator = NDF * G * F;
	float denominator = max((4.0f * NdotL * NdotV), 0.001f);
	float3 BRDF = numerator / denominator;
	float3 specular = BRDF;

	// Calculate result
	float3 kS = F;
	float3 kD = float3(1.0, 1.0, 1.0) - kS;
	kD *= (1.0f - metallic);

	// Only the dir light cast shadow
	float shadowFactor = 1.0f;
	float4 shadowPos = mul(float4(position.xyz,1.0f), gShadowTransform);
	// from ndc to texture 
	float4x4 T = float4x4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	shadowPos = mul(shadowPos, T);
	// lerp
	shadowFactor = CalcShadowFactor(shadowPos);
	float3 color = shadowFactor * (kD * diffuse + kS * specular) * gDirLight.strength * NdotL;
	float3 ambient = albedo.xyz * 0.08;

	[branch]
	if (gSSAOEnabled)
	{
		float width, height, levels;
		gSSAOMap.GetDimensions(width, height);
		float2 uv = float2(pin.PosH.xy) / float2(width, height);
		ambient *= float(gSSAOMap.Sample(gSamLinearWrap,uv).r);
	}

	color += ambient;
	color = pow(color, (1.0 / 2.2));
	return float4(color,1.0);
}
