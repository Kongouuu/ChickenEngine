#pragma once
#include "Helper/DX12CommonHeader.h"
#include "Device.h"
#include "DescriptorHeap.h"
#include "RootSignature.h"
#include "MipMap.h"

namespace ChickenEngine
{
	class MipMapBuffer
	{
	public:
		void Init(ID3D12Resource* srcResource);
		void GenerateMipsf2();

		D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandle() { return mBufferSrvGpuHandle; }
	private:
	    D3D12_RESOURCE_DESC uavDesc;
		Microsoft::WRL::ComPtr<ID3D12Resource> mSrcBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> mBuffer;
		int mSrvOffset;
		D3D12_GPU_DESCRIPTOR_HANDLE mBufferSrvGpuHandle;
		std::vector<int> mUavOffsets;
		std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> mBufferUavGpuHandles;
	};

}

