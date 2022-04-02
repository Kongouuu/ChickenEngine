#include "pch.h"
#include "Renderer/DX12Renderer/FrameBuffer.h"

namespace ChickenEngine
{
	void FrameBuffer::BuildResource(int width, int height, DXGI_FORMAT format, int miplevels)
	{
		mMiplevels = miplevels;

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
		texDesc.Width = max(width,1);
		texDesc.Height = max(height,1);
		texDesc.DepthOrArraySize = 1;
		texDesc.MipLevels = miplevels;
		texDesc.Format = format;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		ThrowIfFailed(Device::device()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&mBuffer)));
		
		if (mSrvOffset < 0)
		{
			mSrvOffset = DescriptorHeapManager::BindSrv(mBuffer.Get());
			mBufferSrvGpuHandle = DescriptorHeapManager::GetSrvUavGpuHandle(mSrvOffset);
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

		//if (bMipmapEnabled)
		//{
		//	for (auto& o : mUavOffset)
		//	{
		//		DescriptorHeapManager::RebindUav(mBuffer.Get(), mUavOffset[o]);
		//	}
		//}

	}

	void FrameBuffer::StartRender(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, const FLOAT* colorRGBA)
	{
		CommandList::cmdList()->RSSetViewports(1, &mScreenViewport);
		CommandList::cmdList()->RSSetScissorRects(1, &mScissorRect);
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBuffer.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
		CommandList::cmdList()->ClearRenderTargetView(mBufferRtvCpuHandle, colorRGBA, 0, nullptr);
		CommandList::cmdList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		CommandList::cmdList()->OMSetRenderTargets(1, &mBufferRtvCpuHandle, true, &dsvHandle);
	}

	void FrameBuffer::EndRender()
	{
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBuffer.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
	}

}
