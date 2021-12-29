#include "pch.h"
#include "Renderer/DescriptorHeap.h"

namespace ChickenEngine
{
	void DescriptorHeapManager::InitDescriptorHeapManager(int swapChainBufferCount)
	{
		LOG_INFO("DescriptorHeapManager - Initialize descriptor heap manager");
		DescriptorHeapManager& dhm = instance();
		dhm.mSwapChainBufferCount = swapChainBufferCount;

		dhm.mRtvDescriptorSize = Device::device()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		dhm.mDsvDescriptorSize = Device::device()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		dhm.mCbvSrvUavDescriptorSize = Device::device()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		
		dhm.mImguiSrvOffset = 0;
		dhm.mNullTexSrvOffset = 1;
		dhm.mNullCubeSrvOffset = 2;
		dhm.mShadowSrvOffset = 3;
		dhm.mTextureSrvOffset= 4;
	
	}

	void DescriptorHeapManager::BuildRtvSrvDsvHeapDesc(int numTex)
	{
		LOG_INFO("DescriptorHeapManager - Build RtvSrvDsv Heap Descriptor. numTex: {0} ", numTex);
		DescriptorHeapManager& dhm = instance();

		// SRV:
		// Imgui(1), null2d(1), nullcube(1), shadow(1), tex(n), cubetex(m), 
		D3D12_DESCRIPTOR_HEAP_DESC SrvHeapDesc;
		SrvHeapDesc.NumDescriptors = 4 + numTex;
		SrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		SrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		SrvHeapDesc.NodeMask = 0;
		ThrowIfFailed(Device::device()->CreateDescriptorHeap(
			&SrvHeapDesc, IID_PPV_ARGS(dhm.mSrvHeap.GetAddressOf())));

		// RTV
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		rtvHeapDesc.NumDescriptors = dhm.mSwapChainBufferCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;
		ThrowIfFailed(Device::device()->CreateDescriptorHeap(
			&rtvHeapDesc, IID_PPV_ARGS(dhm.mRtvHeap.GetAddressOf())));

		// DSV:
		// default + shadowmap
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
		dsvHeapDesc.NumDescriptors = 2;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDesc.NodeMask = 0;
		ThrowIfFailed(Device::device()->CreateDescriptorHeap(
			&dsvHeapDesc, IID_PPV_ARGS(dhm.mDsvHeap.GetAddressOf())));
	}

	void DescriptorHeapManager::BuildCommonSrvHeap()
	{
		LOG_INFO("DescriptorHeap - Build null srv heap");
		DescriptorHeapManager& dhm = instance();

		auto srvCpuStart = dhm.mSrvHeap->GetCPUDescriptorHandleForHeapStart();
		auto srvGpuStart = dhm.mSrvHeap->GetGPUDescriptorHandleForHeapStart();

		// null texture
		auto nullSrv = CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, dhm.mNullTexSrvOffset, dhm.mCbvSrvUavDescriptorSize);
		dhm.mNullTexSrv = CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, dhm.mNullTexSrvOffset, dhm.mCbvSrvUavDescriptorSize);
		dhm.mNullCubeSrv = CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, dhm.mNullCubeSrvOffset, dhm.mCbvSrvUavDescriptorSize);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		Device::device()->CreateShaderResourceView(nullptr, &srvDesc, nullSrv);


		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = 1;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		nullSrv.Offset(1, dhm.mCbvSrvUavDescriptorSize);
		Device::device()->CreateShaderResourceView(nullptr, &srvDesc, nullSrv);


		// shadow
	}

	void DescriptorHeapManager::BuildShadowMapHeap(Microsoft::WRL::ComPtr<ID3D12Resource> shadowMap)
	{
		auto srvCpuStart = instance().mSrvHeap->GetCPUDescriptorHandleForHeapStart();
		auto shadowSrv = CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, instance().mShadowSrvOffset, instance().mCbvSrvUavDescriptorSize);
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		srvDesc.Texture2D.PlaneSlice = 0;
		Device::device()->CreateShaderResourceView(shadowMap.Get(), &srvDesc, shadowSrv);


		auto dsvCpuStart = instance().mDsvHeap->GetCPUDescriptorHandleForHeapStart();
		auto shadowDsv = CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvCpuStart, 1, instance().mDsvDescriptorSize);
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.Texture2D.MipSlice = 0;
		Device::device()->CreateDepthStencilView(shadowMap.Get(), &dsvDesc, shadowDsv);
	}

	void DescriptorHeapManager::BuildTextureSrvHeap(ETextureDimension texType, UINT offset, Microsoft::WRL::ComPtr<ID3D12Resource> resource)
	{
		LOG_INFO("DescriptorHeapManager - Build texture srv heap");
		DescriptorHeapManager& dhm = instance();
		CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(dhm.mSrvHeap->GetCPUDescriptorHandleForHeapStart());
		hDescriptor.Offset(offset + TextureSrvOffset(), dhm.mCbvSrvUavDescriptorSize);
		LOG_TRACE("Tex id: {0},    Tex offset: {1}", offset, offset + TextureSrvOffset());
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
		Device::device()->CreateShaderResourceView(resource.Get(), &srvDesc, hDescriptor);


	}


}

