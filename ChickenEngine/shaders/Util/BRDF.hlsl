static const float PI = 3.14159265359;

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness; 
    float a2 = a * a; 
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float a = roughness;
    float k = (a * a) / 2.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float ggx2 = GeometrySchlickGGX(dot(N, V), roughness);
    float ggx1 = GeometrySchlickGGX(dot(N, L), roughness);

    return ggx1 * ggx2;
}

float3 FresnelSchlick(float3 F0, float3 V, float3 H)
{
    float VdotH = dot(V, H);
    float3 F = F0 + (1.0- F0) * pow((1.0 - VdotH), 5.0);
    return F;
}