#define SM_DEFAULT 1
#define SM_PCF 2
#define SM_PCSS 3
#define SM_VSSM 4
#define SM_MAX_MIPLEVEL 7

#define NUM_SAMPLES 18
#define NUM_RINGS 10

static float2 poissonDisk[NUM_SAMPLES];

// from games 202
void poissonDiskSamples(float2 randomSeed) {

	float ANGLE_STEP = PI2 * float(NUM_RINGS) / float(NUM_SAMPLES);
	float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES);

	float angle = Rand2To1(randomSeed) * PI2;
	float radius = INV_NUM_SAMPLES;
	float radiusStep = radius;

	for (int i = 0; i < NUM_SAMPLES; i++) {
		poissonDisk[i] = float2(cos(angle), sin(angle)) * pow(radius, 0.75);
		radius += radiusStep;
		angle += ANGLE_STEP;
	}
}

// --------------- PCF ------------------
float PCF(float4 shadowPos, uint width, float scale)
{
	// Texel size.
	float offsetScale = (1.0f * scale) / (float)width;

	float percentLit = 0.0f;
	[unroll]
	for (int i = 0; i < NUM_SAMPLES; ++i)
	{
		float2 uv = shadowPos.xy + poissonDisk[i] * offsetScale;
		percentLit += gShadowMap.SampleCmpLevelZero(gSamShadow,uv, shadowPos.z).r;
	}

	return percentLit / ((float)NUM_SAMPLES);
}


// --------------- PCSS ------------------
float findBlocker(float4 shadowPos, uint width, float scale) {
	float totalDepth = 0.0;
	int blockCount = 0;
	float offsetScale = (1.0f * scale) / (float)width;

	[unroll]
	for (int i = 0; i < NUM_SAMPLES; i++) {
		float2 uv = shadowPos.xy + (poissonDisk[i] * offsetScale);
		float shadowMapDepth = gShadowMap.Sample(gSamLinearWrap, uv).r;
		totalDepth = totalDepth + step(shadowMapDepth, shadowPos.z) * shadowMapDepth;
		blockCount = blockCount + step(shadowMapDepth, shadowPos.z);
	}

	if (blockCount == 0) {
		return -1.0;
	}

	if (blockCount == NUM_SAMPLES) {
		return 2.0;
	}

	return totalDepth / float(blockCount);
}

float PCSS(float4 shadowPos, uint width)
{
	// blocker search
	float zocc = findBlocker(shadowPos, width, 2.0f);

	if (zocc < 0.0f)
		return 1.0f;
	if (zocc > 1.0f)
		return 0.0f;

	// penumbra size
	float penumbraSize = (shadowPos.z - zocc) / zocc;

	// pcf
	// penumbra size's adjustment should follow ortho camera's size, will adjust later
	return PCF(shadowPos, width, penumbraSize * 20.0f);
}


// ---------------VSSM ------------------
float linstep(float min, float max, float v)
{
	return clamp((v - min) / (max - min), 0.0f, 1.0f);
}

float ReduceLightBleeding(float p_max, float amount)
{
	return linstep(amount, 1.0f, p_max);
}

float ChebyshevUpperBound(float2 moments, float t) {   
 
	float p = float(t <= moments.x);
	float Variance = moments.y - (moments.x*moments.x);   
	Variance = max(Variance, gVsmMinVar);
	float d = t - moments.x;  
	float p_max = Variance / (Variance + d*d);  

	return max(p, p_max);
} 

float VSM(float4 shadowPos, float scale)
{
	float miplevel = clamp(log2(scale),0.0f,7.0f);
	float2 moments = gShadowMap.SampleLevel(gSamLinearWrap, shadowPos.xy, miplevel).rg;

	float p = ChebyshevUpperBound(moments, shadowPos.z);
	p = ReduceLightBleeding(p, gVsmAmount);
	return  p;
}

float VSSM(float4 shadowPos, uint width)
{
	// blocker search
	float2 moments = gShadowMap.SampleLevel(gSamLinearWrap, shadowPos.xy,1).rg;
	float t = shadowPos.z ;
	float p = ChebyshevUpperBound(moments, t);
	float zocc = max(0.01f,((moments.x - p * t) / (1.0f - p)));

	// if zocc is deeper, or visibility is above 0.90
	if (t < zocc || p >= 0.90)
		return 1.0f;


	//penumbra size
	float penumbraSize = ((t - zocc) / zocc) * 20.f;
	penumbraSize = max(penumbraSize, 2.0f);


	// vsm
	return VSM(shadowPos, penumbraSize);
}

float CalcShadowFactor(float4 shadowPos)
{
	[branch]
	if (gShadowType == 0)
	{
		return 1.0f;
	}

	uint width, height, numMips;
	gShadowMap.GetDimensions(0, width, height, numMips);

	[branch]
	if (gShadowType == SM_PCF)
	{
		poissonDiskSamples(shadowPos.xy);
		return PCF(shadowPos, width, 1.0f);
	}

	[branch]
	if (gShadowType == SM_PCSS)
	{
		poissonDiskSamples(shadowPos.xy);
		return PCSS(shadowPos, width);
	}

	[branch]
	if (gShadowType == SM_VSSM)
	{
		return VSSM(shadowPos, width);
	}

	// return default;
	float shadowMapDepth = gShadowMap.Sample(gSamLinearWrap, shadowPos.xy).r;
	return step(shadowPos.z, shadowMapDepth);

}