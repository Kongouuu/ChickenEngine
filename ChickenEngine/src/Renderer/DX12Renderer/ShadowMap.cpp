#include "pch.h"
#include "Renderer/DX12Renderer/ShadowMap.h"

namespace ChickenEngine
{
	void ShadowMap::Init(uint32_t width, uint32_t height)
	{
		instance().mWidth = width;
		instance().mHeight = height;

		instance().mViewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
		instance().mScissorRect = { 0, 0, (int)width, (int)height };

		instance().BuildResource();
		instance().BuildDescriptors();
		LOG_INFO("");
	}

	void ShadowMap::OnResize(uint32_t width, uint32_t height)
	{
		if ((instance().mWidth != width) || (instance().mHeight != height))
		{
			instance().mWidth = width;
			instance().mHeight = height;
			instance().mViewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
			instance().mScissorRect = { 0, 0, (int)width, (int)height };

			instance().BuildResource();
			instance().BuildDescriptors();
		}
	}

	void ShadowMap::BuildResource()
	{
		D3D12_RESOURCE_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Alignment = 0;
		texDesc.Width = mWidth;
		texDesc.Height = mHeight;
		texDesc.DepthOrArraySize = 1;
		texDesc.MipLevels = 1;
		texDesc.Format = mFormat;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;

		ThrowIfFailed(Device::device()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			&optClear,
			IID_PPV_ARGS(&mShadowMap)));
	}

	void ShadowMap::BuildDescriptors()
	{
		if (mSrvOffset < 0)
		{
			mSrvOffset = DescriptorHeapManager::BindSrv(mShadowMap.Get(), ETextureDimension::TEXTURE2D, DXGI_FORMAT_R24_UNORM_X8_TYPELESS);
			mSrvGpuHandle = DescriptorHeapManager::GetSrvGpuHandle(mSrvOffset);
		}
		else
		{
			if (!DescriptorHeapManager::RebindSrv(mShadowMap.Get(), mSrvOffset))
				assert(0);
		}

		if (mDsvOffset < 0)
		{
			mDsvOffset = DescriptorHeapManager::BindDsv(mShadowMap.Get());
			mDsvCpuHandle = DescriptorHeapManager::GetDsvCpuHandle(mDsvOffset);
		}
		else
		{
			if (!DescriptorHeapManager::RebindDsv(mShadowMap.Get(), mDsvOffset))
				assert(0);
		}

	}

	void ShadowMap::BeginShadowMap(uint32_t passCBByteSize, Microsoft::WRL::ComPtr<ID3D12Resource> passCB)
	{
		PSOManager::UsePSO("shadow");
		auto& cmdList = CommandList::cmdList();
		auto& sm = instance();
		cmdList->RSSetViewports(1, &sm.mViewport);
		cmdList->RSSetScissorRects(1, &sm.mScissorRect);
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(sm.mShadowMap.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));
		cmdList->ClearDepthStencilView(instance().mDsvCpuHandle,
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		cmdList->OMSetRenderTargets(0, nullptr, false, &instance().mDsvCpuHandle);

		D3D12_GPU_VIRTUAL_ADDRESS passCBAddress = passCB->GetGPUVirtualAddress();
		cmdList->SetGraphicsRootConstantBufferView(0, passCBAddress);
	}

	void ShadowMap::EndShadowMap()
	{
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(instance().mShadowMap.Get(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
}