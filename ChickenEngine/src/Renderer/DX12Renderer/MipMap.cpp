#include "pch.h"
#include "Renderer\DX12Renderer\MipMap.h"

namespace ChickenEngine
{
	void MipMapManager::Init()
	{
		if (bInit == true)
			return;
		LOG_INFO("MipMapManager: init");
		bInit = true;
		InitShader();
		InitRootsignature();
		InitPSO();

	}

	void MipMapManager::InitShader()
	{
		LOG_INFO("MipMapManager: init shader");
		ShaderManager::LoadShader("mipmap", "Mipfloat4.hlsl", EShaderType::COMPUTE_SHADER);
		ShaderManager::LoadShader("mipmapf2", "Mipfloat2.hlsl", EShaderType::COMPUTE_SHADER);
	}

	void MipMapManager::InitRootsignature()
	{
		LOG_INFO("MipMapManager: init root signature");
		std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter = std::vector<CD3DX12_ROOT_PARAMETER>(6);


		CD3DX12_DESCRIPTOR_RANGE tex;
		tex.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);


		std::vector<CD3DX12_DESCRIPTOR_RANGE> uavtex = std::vector < CD3DX12_DESCRIPTOR_RANGE>(4);
		for (int i = 0; i < 4; i++)
		{
			uavtex[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, i);
		}
		slotRootParameter[0].InitAsConstants(5, 0);
		slotRootParameter[1].InitAsDescriptorTable(1, &tex); // shader visibility pixel :(
		for (int i = 0; i < 4; i++)
		{
			slotRootParameter[2+i].InitAsDescriptorTable(1, &uavtex[i]);
		}

		RootSignatureManager::instance().CreateRootSignatureFromParam("mipmap", slotRootParameter, 6);
	}

	void MipMapManager::InitPSO()
	{
		LOG_INFO("MipMapManager: init pso");

		D3D12_COMPUTE_PIPELINE_STATE_DESC mipf4PSO = {};
		mipf4PSO.pRootSignature = RootSignatureManager::GetRootSignature("mipmap").Get();
		mipf4PSO.CS =
		{
			reinterpret_cast<BYTE*>(ShaderManager::GetCS("mipmap")->GetBufferPointer()),
			ShaderManager::GetCS("mipmap")->GetBufferSize()
		};
		mipf4PSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		HRESULT hr = Device::device()->CreateComputePipelineState(&mipf4PSO, IID_PPV_ARGS(&mipPSO["mipdefault"]));

		D3D12_COMPUTE_PIPELINE_STATE_DESC mipf2PSO = {};
		mipf2PSO.pRootSignature = RootSignatureManager::GetRootSignature("mipmap").Get();
		mipf2PSO.CS =
		{
			reinterpret_cast<BYTE*>(ShaderManager::GetCS("mipmapf2")->GetBufferPointer()),
			ShaderManager::GetCS("mipmapf2")->GetBufferSize()
		};
		mipf2PSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		ThrowIfFailed(Device::device()->CreateComputePipelineState(&mipf2PSO, IID_PPV_ARGS(&mipPSO["mipfloat2"])));
	}
};