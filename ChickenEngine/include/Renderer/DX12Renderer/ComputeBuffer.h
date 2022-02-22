#pragma once
#include "Helper/DX12CommonHeader.h"
#include "Device.h"
#include "DescriptorHeap.h"
#include "RootSignature.h"
#include "MipMap.h"
#include "BlurFilter.h"

namespace ChickenEngine
{
	class ComputeBuffer
	{
	public:
		void Init(ID3D12Resource* srcResource, D3D12_GPU_DESCRIPTOR_HANDLE srvHandle);
		void BlurToTargetf2();
		void CopyToTarget();
		void GenerateMipsf2();

		D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandle() { return mBufferSrvGpuHandle; }
	private:
		D3D12_RESOURCE_DESC uavDesc;
		Microsoft::WRL::ComPtr<ID3D12Resource> mSrcBuffer;
		D3D12_GPU_DESCRIPTOR_HANDLE mSrcSrvGpuHandle;

		Microsoft::WRL::ComPtr<ID3D12Resource> mTmpBuffer;
		D3D12_GPU_DESCRIPTOR_HANDLE mTmpSrvGpuHandle;
		std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> mTmpUavGpuHandles;

		Microsoft::WRL::ComPtr<ID3D12Resource> mBuffer;
		D3D12_GPU_DESCRIPTOR_HANDLE mBufferSrvGpuHandle;
		std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> mBufferUavGpuHandles;
	};

}

