static const float adaptive_lum = 1.0f;

float3 ReinhardToneMapping(float3 color)
{
    return color / (1.0f + color);
}

float3 ACESToneMapping(float3 color)
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;

	color *= adaptive_lum;
	return (color * (A * color + B)) / (color * (C * color + D) + E);
}

// Also cry engine tone mapping
float3 CryengineExposureMapping(float3 color)
{
	return  float3(1.0) - exp(-color * adaptive_lum);
}

float3 ExposureMapping(float3 color, float exposure)
{
    return  float3(1.0) - exp(-color * exposure);
}