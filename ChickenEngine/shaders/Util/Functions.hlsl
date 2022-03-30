float3 NDCToTexture(float3 f)
{
	return f * 0.5f + float3(0.5f,0.5f,0.5f);
}

float3 TextureToNDC(float3 f)
{
	return f * 2.0f - float3(1.0f,1.0f,1.0f);
}