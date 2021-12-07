#include "pch.h"
#include "Renderer/DescriptorHeap.h"

namespace ChickenEngine
{
	DescriptorHeapManager& DescriptorHeapManager::GetInstance()
	{
		static DescriptorHeapManager instance;
		return instance;
	}

	void DescriptorHeapManager::InitDescriptorHeapManager(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, int swapChainBufferCount)
	{
		LOG_INFO("DescriptorHeapManager - Initialize descriptor heap manager");
		DescriptorHeapManager& dhm = GetInstance();
		dhm.md3dDevice = d3dDevice;
		dhm.mSwapChainBufferCount = swapChainBufferCount;

		dhm.mRtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		dhm.mDsvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		dhm.mCbvSrvUavDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void DescriptorHeapManager::BuildRtvSrvDsvHeapDesc(int numTex2D, int numTex3D)
	{
		LOG_INFO("DescriptorHeapManager - Build RtvSrvDsv Heap Descriptor. numTex2D: {0}  | numTex3D: {1}", numTex2D, numTex3D);
		DescriptorHeapManager& dhm = GetInstance();

		// SRV:
		// Imgui(1), null2d(1), nullcube(1), tex(n), cubetex(m), shadow(1) 
		D3D12_DESCRIPTOR_HEAP_DESC SrvHeapDesc;
		SrvHeapDesc.NumDescriptors = 4 + numTex2D + numTex3D;
		SrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		SrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		SrvHeapDesc.NodeMask = 0;
		ThrowIfFailed(dhm.md3dDevice->CreateDescriptorHeap(
			&SrvHeapDesc, IID_PPV_ARGS(dhm.mSrvHeap.GetAddressOf())));

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		rtvHeapDesc.NumDescriptors = dhm.mSwapChainBufferCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;
		ThrowIfFailed(dhm.md3dDevice->CreateDescriptorHeap(
			&rtvHeapDesc, IID_PPV_ARGS(dhm.mRtvHeap.GetAddressOf())));

		// DSV:
		// default + shadowmap
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
		dsvHeapDesc.NumDescriptors = 2;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDesc.NodeMask = 0;
		ThrowIfFailed(dhm.md3dDevice->CreateDescriptorHeap(
			&dsvHeapDesc, IID_PPV_ARGS(dhm.mDsvHeap.GetAddressOf())));
	}

	void DescriptorHeapManager::BuildTextureSrvHeap(ETextureType texType, UINT offset, Microsoft::WRL::ComPtr<ID3D12Resource> resource)
	{
		LOG_INFO("DescriptorHeapManager - Build texture srv heap");
		DescriptorHeapManager& dhm = GetInstance();
		CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(dhm.mSrvHeap->GetCPUDescriptorHandleForHeapStart());
		hDescriptor.Offset(offset + TextureSrvOffsetStart(), dhm.mCbvSrvUavDescriptorSize);
		//offset
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		srvDesc.Format = resource->GetDesc().Format;
		srvDesc.Texture2D.MipLevels = resource->GetDesc().MipLevels;
		if (texType == TEXTURE3D)
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MostDetailedMip = 0;
			srvDesc.TextureCube.MipLevels = resource->GetDesc().MipLevels;
			srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		}
		dhm.md3dDevice->CreateShaderResourceView(resource.Get(), &srvDesc, hDescriptor);


	}


}

