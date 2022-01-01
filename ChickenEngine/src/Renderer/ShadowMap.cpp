#include "pch.h"
#include "Renderer/ShadowMap.h"

namespace ChickenEngine
{
	void ShadowMap::Init(UINT width, UINT height)
	{
		instance().mWidth = width;
		instance().mHeight = height;

		instance().mViewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
		instance().mScissorRect = { 0, 0, (int)width, (int)height };

		BuildResource();
	}

	void ShadowMap::OnResize(UINT width, UINT height)
	{
		if ((instance().mWidth != width) || (instance().mHeight != height))
		{
			instance().mWidth = width;
			instance().mHeight = height;

			BuildResource();
			BuildDescriptors();
		}
	}

	void ShadowMap::BuildDescriptors()
	{
		DescriptorHeapManager::BuildShadowMapHeap(instance().mShadowMap);
	}

	void ShadowMap::BuildResource()
	{
		D3D12_RESOURCE_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Alignment = 0;
		texDesc.Width = instance().mWidth;
		texDesc.Height = instance().mHeight;
		texDesc.DepthOrArraySize = 1;
		texDesc.MipLevels = 1;
		texDesc.Format = instance().mFormat;
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
			IID_PPV_ARGS(&instance().mShadowMap)));
	}

	void ShadowMap::BeginShadowMap(UINT passCBByteSize, Microsoft::WRL::ComPtr<ID3D12Resource> passCB)
	{
		PSOManager::UsePSO("shadow");
		auto& cmdList = CommandList::cmdList();
		auto& sm = instance();
		cmdList->RSSetViewports(1, &sm.mViewport);
		cmdList->RSSetScissorRects(1, &sm.mScissorRect);
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(sm.mShadowMap.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));
		cmdList->ClearDepthStencilView(DescriptorHeapManager::ShadowDsv(),
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		cmdList->OMSetRenderTargets(0, nullptr, false, &DescriptorHeapManager::ShadowDsv());

		D3D12_GPU_VIRTUAL_ADDRESS passCBAddress = passCB->GetGPUVirtualAddress();
		cmdList->SetGraphicsRootConstantBufferView(0, passCBAddress);
	}

	void ShadowMap::EndShadowMap()
	{
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(instance().mShadowMap.Get(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
}