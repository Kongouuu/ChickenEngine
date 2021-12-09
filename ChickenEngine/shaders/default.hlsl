cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
};

cbuffer cbMaterial : register(b1)
{
	float roughness;
	float metallic;
	float4 color;
};

cbuffer cbPass : register(b2)
{
	float4x4 view;
	float4x4 invView;
	float4x4 proj;
	float4x4 invProj;
	float4x4 viewProj;
	float4x4 invViewProj;
	float3 eyePos;
	float cbPerObjectPad1;
	float2 gRenderTargetSize;
	float2 gInvRenderTargetSize;
	float gNearZ;
	float gFarZ;
};

struct VertexIn
{
	float3 PosL  : POSITION;
	float3 Norm : NORMAL;
	float2 uv : TEXCOORD;
};

struct VertexOut
{ 
	float4 PosH  : SV_POSITION;
	float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogeneous clip space.
	//float4 posW = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	//vout.PosH = mul(posW,viewProj);
	vout.PosH = float4(vin.PosL.x, vin.PosL.y, 0.5f, 1.0f);
	vout.Color = float4(vin.PosL, 1.0f);

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return pin.Color;
}

