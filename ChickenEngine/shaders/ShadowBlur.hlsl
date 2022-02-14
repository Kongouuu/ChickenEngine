#ifndef SEPERABLE_BLUR_KERNEL_SIZE
#define SEPERABLE_BLUR_KERNEL_SIZE 3
#endif

static const int BLUR_KERNEL_BEGIN = SEPERABLE_BLUR_KERNEL_SIZE / -2;
static const int BLUR_KERNEL_END = SEPERABLE_BLUR_KERNEL_SIZE / 2 + 1;
static const float FLOAT_BLUR_KERNEL_SIZE = (float)SEPERABLE_BLUR_KERNEL_SIZE;
//--------------------------------------------------------------------------------------
// defines
//--------------------------------------------------------------------------------------

Texture2D    gShadowMap : register(t0);
Texture2D	 gShadowSquareMap : register(t1)
SamplerState gSamLinearWarp : register(s0);

//--------------------------------------------------------------------------------------
// Input/Output structures
//--------------------------------------------------------------------------------------
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

//--------------------------------------------------------------------------------------
// Pixel shader that performs bump mapping on the final vertex
//--------------------------------------------------------------------------------------
float2 PSBlurX(VertexOut pin) : SV_Target
{
        float2 dep = 0;
        [unroll] for (int x = BLUR_KERNEL_BEGIN; x < BLUR_KERNEL_END; ++x) {
            dep.x += gShadowMap.Sample(gSamLinearWarp,  float3(pin.uv.x, pin.uv.y, 0), int2(x,0)).r;
            dep.y += gShadowSquaredMap.Sample(gSamLinearWarp, float3(pin.uv.x, pin.uv.y, 0), int2(x, 0)).r;
        }
        dep /= FLOAT_BLUR_KERNEL_SIZE;
        return dep;
}

//--------------------------------------------------------------------------------------
// Pixel shader that performs bump mapping on the final vertex
float2 PSBlurY(VertexOut pin) : SV_Target
{
        float2 dep = 0;
        [unroll] for (int x = BLUR_KERNEL_BEGIN; y < BLUR_KERNEL_END; ++y) {
            dep.x += gShadowMap.Sample(gSamLinearWarp,  float3(pin.uv.x, pin.uv.y, 0), int2(x,0)).r;
            dep.y += gShadowSquaredMap.Sample(gSamLinearWarp, float3(pin.uv.x, pin.uv.y, 0), int2(0, y)).r;
        }
        dep /= FLOAT_BLUR_KERNEL_SIZE;
        return dep;
}