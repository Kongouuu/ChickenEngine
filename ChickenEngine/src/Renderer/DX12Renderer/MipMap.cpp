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

		uavResources = std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>(3);
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

	bool MipMapManager::GenerateMipsf2(ID3D12Resource* resource, D3D12_GPU_DESCRIPTOR_HANDLE srvHandle)
	{
		if (!resource || !bInit)
			return false;
		D3D12_RESOURCE_DESC uavDesc = resource->GetDesc();
		if (uavDesc.MipLevels == 1)
			return false;

		// UAV
		uavResources[curResource].Reset();
		uavDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		uavDesc.Flags &= ~(D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		ThrowIfFailed(Device::device()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&uavDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&uavResources[curResource])));
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource,
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_SOURCE));
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(uavResources[curResource].Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
		CommandList::cmdList()->CopyResource(uavResources[curResource].Get(), resource);
		int srvOffset = DescriptorHeapManager::BindSrv(uavResources[curResource].Get());
		std::vector<uint32_t> uavOffsets;
		for (int i = 0; i < uavDesc.MipLevels; i++)
		{
			uavOffsets.push_back(DescriptorHeapManager::BindUav(uavResources[curResource].Get(), i));
		}
		UINT width = uavDesc.Width;
		UINT height = uavDesc.Height;
		UINT mipLevels = uavDesc.MipLevels;
		CommandList::cmdList()->SetComputeRootSignature(RootSignatureManager::GetRootSignature("mipmap").Get());
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(uavResources[curResource].Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
		CommandList::cmdList()->SetComputeRootDescriptorTable(1, DescriptorHeapManager::GetSrvUavGpuHandle(srvOffset));

		int topMip = 0;
		while ( topMip < mipLevels - 1)
		{
			CommandList::cmdList()->SetPipelineState(mipPSO["mipfloat2"].Get());

			uint32_t srcWidth = width >> topMip;
			uint32_t srcHeight = height >> topMip;
			uint32_t dstWidth = static_cast<uint32_t>(srcWidth >> 1);
			uint32_t dstHeight = srcHeight >> 1;

			if ((srcWidth & 1) || (srcHeight & 1))
				return false;

			DWORD mipCount;
			_BitScanForward(&mipCount, (dstWidth == 1 ? dstHeight : dstWidth) |
				(dstHeight == 1 ? dstWidth : dstHeight));
			// Maximum number of mips to generate is 4.
			mipCount = std::min<DWORD>(4, mipCount + 1);
			// Clamp to total number of mips left over.
			mipCount = (topMip + mipCount) >= mipLevels ?
				mipLevels - topMip - 1 : mipCount;
			dstWidth = std::max<DWORD>(1, dstWidth);
			dstHeight = std::max<DWORD>(1, dstHeight);
			UINT SrcMipLevel = topMip;
			UINT NumMipLevels = mipCount;
			UINT SrcDimension = 0;
			float texelSizeX = 1.0f / (float)dstWidth;
			float texelSizeY = 1.0f / (float)dstHeight;
			CommandList::cmdList()->SetComputeRoot32BitConstants(0, 1, &SrcMipLevel, 0);
			CommandList::cmdList()->SetComputeRoot32BitConstants(0, 1, &NumMipLevels, 1);
			CommandList::cmdList()->SetComputeRoot32BitConstants(0, 1, &SrcDimension, 2);
			CommandList::cmdList()->SetComputeRoot32BitConstants(0, 1, &texelSizeX, 3);
			CommandList::cmdList()->SetComputeRoot32BitConstants(0, 1, &texelSizeY, 4);
			for (int i = 1; i <= mipCount; i++)
			{
				CommandList::cmdList()->SetComputeRootDescriptorTable(i+1, DescriptorHeapManager::GetSrvUavGpuHandle(uavOffsets[i+topMip]));
			}

			UINT groupX = (UINT)ceilf(dstWidth / 8.0f);
			UINT groupY = (UINT)ceilf(dstHeight / 8.0f);
			CommandList::cmdList()->Dispatch(groupX, groupY, 1);
			topMip += mipCount;
		}
		

		// clean
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource,
			D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST));
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(uavResources[curResource].Get(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE));

		CommandList::cmdList()->CopyResource(resource, uavResources[curResource].Get());

		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource,
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
		DescriptorHeapManager::UnbindSrv(srvOffset);
		for (int i = 0; i < uavDesc.MipLevels; i++)
		{
			DescriptorHeapManager::UnbindUav(uavOffsets[i]);
		}
		curResource = (curResource+1)%3;
		return true;
	}
};