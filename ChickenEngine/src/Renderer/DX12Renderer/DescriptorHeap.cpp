#include "pch.h"
#include "Renderer/DX12Renderer/DescriptorHeap.h"

namespace ChickenEngine
{
	void DescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors, bool bShaderVisible)
	{
		mHeapDesc.NumDescriptors = numDescriptors;
		mHeapDesc.Type = type;
		mHeapDesc.Flags = bShaderVisible? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(Device::device()->CreateDescriptorHeap(
			&mHeapDesc, IID_PPV_ARGS(mHeap.GetAddressOf())));
		mIncrementSize = Device::device()->GetDescriptorHandleIncrementSize(type);
		mGpuHeapStart = mHeap->GetGPUDescriptorHandleForHeapStart();
		mCpuHeapStart = mHeap->GetCPUDescriptorHandleForHeapStart();
	}

	void DescriptorHeapManager::Init(int swapChainBufferCount)
	{
		mSwapChainBufferCount = swapChainBufferCount;


		mSrvAvailable = std::vector<uint32_t>(srvSize);
		for (size_t i = 0; i < srvSize; i++)
		{
			mSrvAvailable[i] = srvSize - i - 1;
		}
		mSrvAvailable.pop_back();

		// save 2 for back buffer
		mRtvAvailable = std::vector<uint32_t>(rtvSize);
		for (size_t i = 0; i < rtvSize; i++)
		{
			mRtvAvailable[i] = rtvSize - i - 1;
		}
		for(int i=0; i<swapChainBufferCount; i++)
			mRtvAvailable.pop_back();
		
		// save 1 for back buffer
		mDsvAvailable = std::vector<uint32_t>(dsvSize);
		for (size_t i = 0; i < dsvSize; i++)
		{
			mDsvAvailable[i] = dsvSize - i - 1;
		}
		mDsvAvailable.pop_back();

		LOG_INFO("srv last {0}, rtv last {1}, dsv last {2}", mSrvAvailable.back(), mRtvAvailable.back(), mDsvAvailable.back());
	}

	void DescriptorHeapManager::BuildDesc()
	{
		mSrvHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, srvSize, true);
		mRtvHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, rtvSize);
		mDsvHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_DSV,dsvSize);
	}

	uint32_t DescriptorHeapManager::BindSrv(ID3D12Resource* resource, ETextureDimension dimension)
	{
		if (resource!=nullptr)
		{
			return BindSrv(resource, dimension, resource->GetDesc().Format, resource->GetDesc().MipLevels);
		}
		else
		{
			return BindSrv(resource, dimension, DXGI_FORMAT_R8G8B8A8_UNORM, 1);
		}
	
		return 0;
	}

	uint32_t DescriptorHeapManager::BindSrv(ID3D12Resource* resource, ETextureDimension dimension, DXGI_FORMAT format, UINT16 mipLevels)
	{
		if (instance().mSrvAvailable.empty())
		{
			LOG_ERROR("DescriptorHeapManager: No available srv descriptor slot left");
			assert(0);
		}

		uint32_t offset = instance().mSrvAvailable.back();
		instance().mSrvAvailable.pop_back();
		LOG_INFO("DescriptorHeapManager: Binding srv heap, offset {0}", offset);
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0;
		srvDesc.Format = format;
		srvDesc.Texture2D.MipLevels = mipLevels;

		if (dimension == ETextureDimension::TEXTURE3D)
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MostDetailedMip = 0;
			srvDesc.TextureCube.MipLevels = mipLevels;
			srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		}
		instance().mSrvDescByOffset[offset] = srvDesc;
		LOG_INFO("sizeof srvdesc: {0}, sizeof int{1}", sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC), sizeof(int));
		Device::device()->CreateShaderResourceView(resource, &srvDesc, instance().mSrvHeap.GetCpuHandle(offset));
		return offset;
	}

	uint32_t DescriptorHeapManager::BindRtv(ID3D12Resource* resource, bool bBackBuffer, uint32_t backBufferID)
	{
		if (instance().mRtvAvailable.empty())
		{
			LOG_ERROR("DescriptorHeapManager: No available rtv descriptor slot left");
			assert(0);
		}

		uint32_t offset; 

		if (bBackBuffer)
		{
			offset = backBufferID;
			Device::device()->CreateRenderTargetView(resource, nullptr, instance().mRtvHeap.GetCpuHandle(offset));
		}
		else
		{
			offset = instance().mRtvAvailable.back();
			instance().mRtvAvailable.pop_back();
			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Format = resource->GetDesc().Format;
			rtvDesc.Texture2D.MipSlice = 0;
			rtvDesc.Texture2D.PlaneSlice = 0;
			instance().mRtvDescByOffset[offset] = rtvDesc;
			Device::device()->CreateRenderTargetView(resource, &rtvDesc, instance().mRtvHeap.GetCpuHandle(offset));
		}
		LOG_INFO("DescriptorHeapManager: Binding rtv heap, offset {0}", offset);
		return offset;
	}

	uint32_t DescriptorHeapManager::BindDsv(ID3D12Resource* resource, bool bBackBuffer)
	{
		if (instance().mDsvAvailable.empty())
		{
			LOG_ERROR("DescriptorHeapManager: No available dsv descriptor slot left");
			assert(0);
		}

		uint32_t offset = 0; 

		if (bBackBuffer)
		{
			Device::device()->CreateDepthStencilView(resource, nullptr, instance().mDsvHeap.GetCpuHandle(0));
		}
		else
		{
			offset = instance().mDsvAvailable.back();
			instance().mDsvAvailable.pop_back();
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
			dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			dsvDesc.Texture2D.MipSlice = 0;
			instance().mDsvDescByOffset[offset] = dsvDesc;
			Device::device()->CreateDepthStencilView(resource, &dsvDesc, instance().mDsvHeap.GetCpuHandle(offset));
		}
		return offset;
	}

	bool DescriptorHeapManager::RebindSrv(ID3D12Resource* resource, uint32_t offset)
	{
		// 检查这个偏移是不是真的有已储存的描述符
		if (instance().mSrvDescByOffset.find(offset) == instance().mSrvDescByOffset.end())
			return false;
		Device::device()->CreateShaderResourceView(resource, &instance().mSrvDescByOffset[offset], instance().mSrvHeap.GetCpuHandle(offset));
		return true;
	}

	bool DescriptorHeapManager::RebindRtv(ID3D12Resource* resource, uint32_t offset)
	{
		if (instance().mRtvDescByOffset.find(offset) == instance().mRtvDescByOffset.end())
			return false;
		Device::device()->CreateRenderTargetView(resource, &instance().mRtvDescByOffset[offset], instance().mRtvHeap.GetCpuHandle(offset));
		return true;
	}

	bool DescriptorHeapManager::RebindDsv(ID3D12Resource* resource, uint32_t offset)
	{
		if (instance().mDsvDescByOffset.find(offset) == instance().mDsvDescByOffset.end())
			return false;
		Device::device()->CreateDepthStencilView(resource, &instance().mDsvDescByOffset[offset], instance().mDsvHeap.GetCpuHandle(offset));
		return true;
	}

	void DescriptorHeapManager::UnbindSrv(uint32_t offset)
	{
		instance().mSrvAvailable.push_back(offset);
	}

	void DescriptorHeapManager::UnbindRtv(uint32_t offset)
	{
		instance().mRtvAvailable.push_back(offset);
	}

	void DescriptorHeapManager::UnbindDsv(uint32_t offset)
	{
		instance().mDsvAvailable.push_back(offset);
	}
}