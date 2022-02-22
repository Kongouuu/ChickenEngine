#include "pch.h"
#include "Renderer/DX12Renderer/BlurFilter.h"

namespace ChickenEngine
{
	std::vector<float> BlurFilterManager::CalcGaussWeight(float sigma)
	{
		float twoSigma2 = 2.0f * sigma * sigma;

		// Estimate the blur radius based on sigma since sigma controls the "width" of the bell curve.
		// For example, for sigma = 3, the width of the bell curve is 
		int blurRadius = (int)ceil(2.0f * sigma);

		assert((int)blurRadius <= instance().MaxBlurRadius);

		std::vector<float> weights;
		weights.resize(2 * blurRadius + 1);

		float weightSum = 0.0f;

		for (int i = -blurRadius; i <= blurRadius; ++i)
		{
			float x = (float)i;

			weights[i + blurRadius] = expf(-x * x / twoSigma2);

			weightSum += weights[i + blurRadius];
		}

		// Divide by the sum so all the weights add up to 1.0.
		for (int i = 0; i < weights.size(); ++i)
		{
			weights[i] /= weightSum;
		}

		return weights;
	}

	void BlurFilterManager::Init()
	{
		if (bInit == true)
			return;
		LOG_INFO("BlurFilterManager: init");
		bInit = true;
		InitShader();
		InitRootsignature();
		InitPSO();

	}

	void BlurFilterManager::InitShader()
	{
		LOG_INFO("BlurFilterManager: init shader");
		ShaderManager::LoadShader("blurHorzf2", "BlurFloat2.hlsl", EShaderType::COMPUTE_SHADER, "HorzBlurCS"); 
		ShaderManager::LoadShader("blurVertf2", "BlurFloat2.hlsl", EShaderType::COMPUTE_SHADER, "VertBlurCS");
	}

	void BlurFilterManager::InitRootsignature()
	{
		LOG_INFO("BlurFilterManager: init root signature");
		std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter = std::vector<CD3DX12_ROOT_PARAMETER>(3);


		CD3DX12_DESCRIPTOR_RANGE srvtex;
		srvtex.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);


		CD3DX12_DESCRIPTOR_RANGE uavtex;
		uavtex.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

		slotRootParameter[0].InitAsConstants(12, 0);
		slotRootParameter[1].InitAsDescriptorTable(1, &srvtex); 
		slotRootParameter[2].InitAsDescriptorTable(1, &uavtex);

		RootSignatureManager::instance().CreateRootSignatureFromParam("blurfilter", slotRootParameter, 3);
	}

	void BlurFilterManager::InitPSO()
	{
		LOG_INFO("BlurFilterManager: init pso");

		D3D12_COMPUTE_PIPELINE_STATE_DESC blurPSODesc= {};
		blurPSODesc.pRootSignature = RootSignatureManager::GetRootSignature("blurfilter").Get();
		blurPSODesc.CS =
		{
			reinterpret_cast<BYTE*>(ShaderManager::GetCS("blurHorzf2")->GetBufferPointer()),
			ShaderManager::GetCS("blurHorzf2")->GetBufferSize()
		};
		blurPSODesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		ThrowIfFailed(Device::device()->CreateComputePipelineState(&blurPSODesc, IID_PPV_ARGS(&blurPSO["horzf2"])));
		blurPSODesc.CS =
		{
			reinterpret_cast<BYTE*>(ShaderManager::GetCS("blurVertf2")->GetBufferPointer()),
			ShaderManager::GetCS("blurVertf2")->GetBufferSize()
		};
		ThrowIfFailed(Device::device()->CreateComputePipelineState(&blurPSODesc, IID_PPV_ARGS(&blurPSO["vertf2"])));
	}
};