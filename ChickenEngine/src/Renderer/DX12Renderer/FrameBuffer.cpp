#include "pch.h"
#include "Renderer/DX12Renderer/FrameBuffer.h"

namespace ChickenEngine
{
	void FrameBuffer::BuildResource(int width, int height, DXGI_FORMAT format)
	{
		mScreenViewport.TopLeftX = 0;
		mScreenViewport.TopLeftY = 0;
		mScreenViewport.Width = static_cast<float>(width);
		mScreenViewport.Height = static_cast<float>(height);
		mScreenViewport.MinDepth = 0.0f;
		mScreenViewport.MaxDepth = 1.0f;
		mScissorRect = { 0, 0, (long)width, (long)height };

		D3D12_RESOURCE_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Alignment = 0;
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.DepthOrArraySize = 1;
		texDesc.MipLevels = 1;
		texDesc.Format = format;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		XMFLOAT4 clearColor;
		XMStoreFloat4(&clearColor, DirectX::Colors::LightSteelBlue);
		D3D12_CLEAR_VALUE optClear;
		optClear.Format = format;
		optClear.Color[0] = clearColor.x;
		optClear.Color[1] = clearColor.y;
		optClear.Color[2] = clearColor.z;
		optClear.Color[3] = clearColor.w;

		ThrowIfFailed(Device::device()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			&optClear,
			IID_PPV_ARGS(&mBuffer)));
		
		if (mSrvOffset < 0)
		{
			mSrvOffset = DescriptorHeapManager::BindSrv(mBuffer.Get());
			mBufferSrvGpuHandle = DescriptorHeapManager::GetSrvGpuHandle(mSrvOffset);
		}
		else
		{
			if (!DescriptorHeapManager::RebindSrv(mBuffer.Get(), mSrvOffset))
				assert(0);
		}

		if (mRtvOffset < 0)
		{
			mRtvOffset = DescriptorHeapManager::BindRtv(mBuffer.Get());
			mBufferRtvCpuHandle = DescriptorHeapManager::GetRtvCpuHandle(mRtvOffset);
		}
		else
		{
			if (!DescriptorHeapManager::RebindRtv(mBuffer.Get(), mRtvOffset))
				assert(0);
		}

	}

	void FrameBuffer::StartRender(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
	{
		CommandList::cmdList()->RSSetViewports(1, &mScreenViewport);
		CommandList::cmdList()->RSSetScissorRects(1, &mScissorRect);
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBuffer.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
		CommandList::cmdList()->ClearRenderTargetView(mBufferRtvCpuHandle, DirectX::Colors::LightSteelBlue, 0, nullptr);
		CommandList::cmdList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		CommandList::cmdList()->OMSetRenderTargets(1, &mBufferRtvCpuHandle, true, &dsvHandle);
	}

	void FrameBuffer::EndRender()
	{
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBuffer.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
	}

}