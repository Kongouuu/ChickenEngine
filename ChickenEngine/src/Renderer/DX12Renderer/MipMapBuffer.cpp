#include "pch.h"
#include "Renderer/DX12Renderer/MipMapBuffer.h"

namespace ChickenEngine
{
	void MipMapBuffer::Init(ID3D12Resource* srcResource)
	{
		if (!srcResource)
			return;
		uavDesc = srcResource->GetDesc();
		if (uavDesc.MipLevels == 1)
			return;
		mSrcBuffer = srcResource;
		uavDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		uavDesc.Flags &= ~(D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		ThrowIfFailed(Device::device()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&uavDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&mBuffer)));

		mSrvOffset = DescriptorHeapManager::BindSrv(mBuffer.Get());
		mBufferSrvGpuHandle = DescriptorHeapManager::GetSrvUavGpuHandle(mSrvOffset);

		for (int i = 0; i < uavDesc.MipLevels; i++)
		{
			mUavOffsets.push_back(DescriptorHeapManager::BindUav(mBuffer.Get(), i));
			mBufferUavGpuHandles.push_back(DescriptorHeapManager::GetSrvUavGpuHandle(mUavOffsets[i]));
		}
	}

	void MipMapBuffer::GenerateMipsf2()
	{
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mSrcBuffer.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_SOURCE));
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBuffer.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));
		CommandList::cmdList()->CopyResource(mBuffer.Get(), mSrcBuffer.Get());
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mSrcBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_GENERIC_READ));

		UINT width = uavDesc.Width;
		UINT height = uavDesc.Height;
		UINT mipLevels = uavDesc.MipLevels;
		CommandList::cmdList()->SetComputeRootSignature(RootSignatureManager::GetRootSignature("mipmap").Get());
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
		CommandList::cmdList()->SetComputeRootDescriptorTable(1, mBufferSrvGpuHandle);

		int topMip = 0;
		while (topMip < mipLevels - 1)
		{
			CommandList::cmdList()->SetPipelineState(MipMapManager::instance().GetMipPSO()["mipfloat2"].Get());

			uint32_t srcWidth = width >> topMip;
			uint32_t srcHeight = height >> topMip;
			uint32_t dstWidth = static_cast<uint32_t>(srcWidth >> 1);
			uint32_t dstHeight = srcHeight >> 1;

			if ((srcWidth & 1) || (srcHeight & 1))
				return ;

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
				CommandList::cmdList()->SetComputeRootDescriptorTable(i + 1, mBufferUavGpuHandles[i+topMip]);
			}

			UINT groupX = (UINT)ceilf(dstWidth / 8.0f);
			UINT groupY = (UINT)ceilf(dstHeight / 8.0f);
			CommandList::cmdList()->Dispatch(groupX, groupY, 1);
			topMip += mipCount;
		}

		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBuffer.Get(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ));
	}

}
