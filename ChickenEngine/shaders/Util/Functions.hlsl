
#define EPS 1e-3
#define PI 3.141592653589793
#define PI2 6.283185307179586

float3 NDCToTexture(float3 f)
{
	return f * 0.5f + float3(0.5f,0.5f,0.5f);
}

float3 TextureToNDC(float3 f)
{
	return f * 2.0f - float3(1.0f,1.0f,1.0f);
}

// from games 202
float Rand2To1(float2 uv) {
	// 0 - 1
	const float a = 12.9898, b = 78.233, c = 43758.5453;
	float dt = dot(uv.xy, float2(a, b)), sn = fmod(dt, PI);
	return frac(sin(sn) * c);
}

#define NEWTON_ITER 2
#define HALLEY_ITER 0

// https://www.shadertoy.com/view/wts3RX
float cbrt(float x)
{
    float y = sign(x) * asfloat(asuint(abs(x)) / 3u + 0x2a514067u);

    int i;
    [unroll]
    for (i = 0; i < NEWTON_ITER; ++i)
    {
        y = (2. * y + x / (y * y)) * .333333333;
    }

    [unroll]
    for (i = 0; i < HALLEY_ITER; ++i)
    {
        float y3 = y * y * y;
        y *= (y3 + 2. * x) / (2. * y3 + x);
    }

    return y;
}

float3 RandSampleSphere(float2 uv)
{
    float theta = uv.x * 2.0f * PI;
    float phi = acos(2.0f * uv.y - 1.0f);
    float r = cbrt(Rand2To1(uv));
    float x = r * sin(phi) * cos(theta);
    float y = r * sin(phi) * sin(theta);
    float z = r * cos(phi);
    return float3(x, y, z);
}