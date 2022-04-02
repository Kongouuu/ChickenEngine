#include "Util/Common.hlsl"
#include "Util/Functions.hlsl"

Texture2D    gGPositionMap : register(t2);
Texture2D    gRandomVecMap : register(t3);
Texture2D    gGNormalMap : register(t4);


struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float2 uv : TEXCOORD;
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

	vout.uv = gTexCoords[vid];

	// Quad covering screen in NDC space.
	vout.PosH = float4(2.0f * vout.uv.x - 1.0f, 1.0f - 2.0f * vout.uv.y, 0.0f, 1.0f);

	return vout;
}

#define NUM_SAMPLES 14
static float4 offsets[14] =
{
	float4(1.f,1.f,1.f,0.f),
	float4(-1.f,-1.f,-1.f,0.f),
	float4(-1.f,1.f,1.f,0.f),
	float4(1.f,-1.f,-1.f,0.f),
	float4(1.f,1.f,-1.f,0.f),
	float4(-1.f,-1.f,1.f,0.f),
	float4(-1.f,1.f,-1.f,0.f),
	float4(1.f,-1.f,1.f,0.f),
	float4(-1.f,0.f,0.f,0.f),
	float4(1.f,0.f,0.f,0.f),
	float4(0.f,-1.f,0.f,0.f),
	float4(0.f,1.f,0.f,0.f),
	float4(0.f,0.f,-1.f,0.f),
	float4(0.f,0.f,1.f,0.f)
};

#define OCCLUSION_RADIUS 0.3
#define DIST_FALLOFF 10
float4 PS(VertexOut pin) : SV_Target
{
	if (abs(float(gGPositionMap.Sample(gSamLinearWrap, pin.uv).a) - 1.0f) < 0.001f)
	{
		clip(-1);
	}

	float2 uvTmp = pin.uv;
	[unroll]
	for (int i = 0; i < NUM_SAMPLES; ++i)
	{
		offsets[i] *= lerp(0.25f,1.0f,Rand2To1(uvTmp));
		uvTmp *= 13;
	}

	float3 worldPos = float3(gGPositionMap.Sample(gSamLinearWrap, pin.uv).xyz);
	float3 normal = float3(gGNormalMap.Sample(gSamLinearWrap, pin.uv).xyz);

	float visibility = 0.0f;

	float3 randVec = 2.0f * float3(gRandomVecMap.Sample(gSamLinearWrap, pin.uv).rgb) - 1.0f;

	[unroll]
	for (int j = 0; j < NUM_SAMPLES; j++)
	{
		// Get sample position
		float3 offset = reflect(offsets[j].xyz, randVec);
		float flip = sign(dot(offset, normal));
		float4 samplePos = float4(worldPos + flip * OCCLUSION_RADIUS * offset , 1.0f);

		// Get sample depth
		samplePos = mul(samplePos, gViewProj);
		float sampleDepth= samplePos.w;

		// Get sample UV coords
		samplePos /= samplePos.w;
		float2 uv = samplePos.xy * 0.5f + 0.5f;
		uv.y = 1 - uv.y;

		// Get depth in gbuffer
		float depthBuffer = gGPositionMap.Sample(gSamLinearWrap, uv).a;
		
		// Should initialize gbuffer position.w to camera far plane dist.
		depthBuffer = (abs(depthBuffer - 1.0f) < 0.001f) ? -900 : depthBuffer;

		// Consider distance fall off and also blocking or not
		visibility += (abs(depthBuffer - sampleDepth) > DIST_FALLOFF) || (depthBuffer > sampleDepth);
	}
	

	visibility /= float( NUM_SAMPLES);

	return float4(visibility, visibility, visibility,1.0f);
}
