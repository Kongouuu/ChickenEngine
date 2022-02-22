#include "pch.h"
#include "Renderer/DX12Renderer/ComputeBuffer.h"

namespace ChickenEngine
{
	void ComputeBuffer::Init(ID3D12Resource* srcResource, D3D12_GPU_DESCRIPTOR_HANDLE srvHandle)
	{
		if (!srcResource)
			return;
		uavDesc = srcResource->GetDesc();
		if (uavDesc.MipLevels == 1)
			return;
		mSrcBuffer = srcResource;
		mSrcSrvGpuHandle = srvHandle;
		uavDesc.DepthOrArraySize = 1;
		uavDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		uavDesc.Flags &= ~(D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		int offset;

		ThrowIfFailed(Device::device()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&uavDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&mBuffer)));
		offset = DescriptorHeapManager::BindSrv(mBuffer.Get());
		mBufferSrvGpuHandle = DescriptorHeapManager::GetSrvUavGpuHandle(offset);
		for (int i = 0; i < uavDesc.MipLevels; i++)
		{
			offset = DescriptorHeapManager::BindUav(mBuffer.Get(), i);
			mBufferUavGpuHandles.push_back(DescriptorHeapManager::GetSrvUavGpuHandle(offset));
		}

		ThrowIfFailed(Device::device()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&uavDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&mTmpBuffer)));
		offset = DescriptorHeapManager::BindSrv(mTmpBuffer.Get());
		mTmpSrvGpuHandle = DescriptorHeapManager::GetSrvUavGpuHandle(offset);
		for (int i = 0; i < uavDesc.MipLevels; i++)
		{
			offset = DescriptorHeapManager::BindUav(mTmpBuffer.Get(), i);
			mTmpUavGpuHandles.push_back(DescriptorHeapManager::GetSrvUavGpuHandle(offset));
		}
	}

	void ComputeBuffer::BlurToTargetf2()
	{
		auto weights = BlurFilterManager::CalcGaussWeight(1.5f);
		int blurRadius = (int)weights.size() / 2;

		auto& cmdList = CommandList::cmdList();
		cmdList->SetComputeRootSignature(RootSignatureManager::GetRootSignature("blurfilter").Get());

		cmdList->SetComputeRoot32BitConstants(0, 1, &blurRadius, 0);
		cmdList->SetComputeRoot32BitConstants(0, (UINT)weights.size(), weights.data(), 1);

		//
		// Horizontal Blur pass.
		//
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mTmpBuffer.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

		cmdList->SetPipelineState(BlurFilterManager::instance().GetBlurPSO()["horzf2"].Get());
		cmdList->SetComputeRootDescriptorTable(1, mSrcSrvGpuHandle);
		cmdList->SetComputeRootDescriptorTable(2, mTmpUavGpuHandles[0]);

		// How many groups do we need to dispatch to cover a row of pixels, where each
		// group covers 256 pixels (the 256 is defined in the ComputeShader).
		UINT numGroupsX = (UINT)ceilf(uavDesc.Width / 256.0f);
		cmdList->Dispatch(numGroupsX, uavDesc.Height, 1);


		//
		// Vertical Blur pass.
		//
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBuffer.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mTmpBuffer.Get(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ));
		cmdList->SetPipelineState(BlurFilterManager::instance().GetBlurPSO()["vertf2"].Get());

		cmdList->SetComputeRootDescriptorTable(1, mTmpSrvGpuHandle);
		cmdList->SetComputeRootDescriptorTable(2, mBufferUavGpuHandles[0]);

		// How many groups do we need to dispatch to cover a column of pixels, where each
		// group covers 256 pixels  (the 256 is defined in the ComputeShader).
		UINT numGroupsY = (UINT)ceilf(uavDesc.Height / 256.0f);
		cmdList->Dispatch(uavDesc.Width , numGroupsY, 1);
	}

	void ComputeBuffer::CopyToTarget()
	{
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mSrcBuffer.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_SOURCE));
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBuffer.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));
		CommandList::cmdList()->CopyResource(mBuffer.Get(), mSrcBuffer.Get());
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mSrcBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_GENERIC_READ));
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

	}

	void ComputeBuffer::GenerateMipsf2()
	{
		UINT width = uavDesc.Width;
		UINT height = uavDesc.Height;
		UINT mipLevels = uavDesc.MipLevels;
		auto& cmdList = CommandList::cmdList();
		cmdList->SetComputeRootSignature(RootSignatureManager::GetRootSignature("mipmap").Get());

		cmdList->SetComputeRootDescriptorTable(1, mBufferSrvGpuHandle);

		int topMip = 0;
		while (topMip < mipLevels - 1)
		{
			cmdList->SetPipelineState(MipMapManager::instance().GetMipPSO()["mipfloat2"].Get());

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
			cmdList->SetComputeRoot32BitConstants(0, 1, &SrcMipLevel, 0);
			cmdList->SetComputeRoot32BitConstants(0, 1, &NumMipLevels, 1);
			cmdList->SetComputeRoot32BitConstants(0, 1, &SrcDimension, 2);
			cmdList->SetComputeRoot32BitConstants(0, 1, &texelSizeX, 3);
			cmdList->SetComputeRoot32BitConstants(0, 1, &texelSizeY, 4);
			for (int i = 1; i <= mipCount; i++)
			{
				cmdList->SetComputeRootDescriptorTable(i + 1, mBufferUavGpuHandles[i+topMip]);
			}

			UINT groupX = (UINT)ceilf(dstWidth / 8.0f);
			UINT groupY = (UINT)ceilf(dstHeight / 8.0f);
			cmdList->Dispatch(groupX, groupY, 1);
			topMip += mipCount;
		}

		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBuffer.Get(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ));
	}

}
