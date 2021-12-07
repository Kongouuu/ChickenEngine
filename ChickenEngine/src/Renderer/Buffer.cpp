#include "pch.h"
#include "Renderer/Buffer.h"

namespace ChickenEngine
{
	BufferManager& BufferManager::GetInstance()
	{
		static BufferManager instance;
		return instance;
	}

	void BufferManager::InitBufferManager(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList)
	{
        LOG_INFO("BufferManager - Init buffer manager");
		GetInstance().md3dDevice = d3dDevice;
		GetInstance().mCommandList = cmdList;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> BufferManager::CreateDefaultBuffer(const void* initData, UINT64 byteSize, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
	{
        LOG_INFO("BufferManager - Create default buffer");
        Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer;

        BufferManager& bm = GetInstance();

        // Actual buffer
        auto heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        auto buffer = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
        ThrowIfFailed(bm.md3dDevice->CreateCommittedResource(
            &heapProperty,
            D3D12_HEAP_FLAG_NONE,
            &buffer,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

        // Upload heap
        heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        ThrowIfFailed(bm.md3dDevice->CreateCommittedResource(
            &heapProperty,
            D3D12_HEAP_FLAG_NONE,
            &buffer,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(uploadBuffer.GetAddressOf())));


        // Describe the data we want to copy into the default buffer.
        D3D12_SUBRESOURCE_DATA subResourceData = {};
        subResourceData.pData = initData;
        subResourceData.RowPitch = byteSize;
        subResourceData.SlicePitch = subResourceData.RowPitch;


        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
        bm.mCommandList->ResourceBarrier(1, &barrier);

        UpdateSubresources<1>(bm.mCommandList.Get(), defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);

        barrier = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
        bm.mCommandList->ResourceBarrier(1, &barrier);



        return defaultBuffer;
	}
}