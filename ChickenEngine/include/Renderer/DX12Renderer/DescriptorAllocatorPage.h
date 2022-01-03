#pragma once
#include "pch.h"
#include "DescriptorAllocation.h"
#include "Helper/DX12CommonHeader.h"

namespace ChickenEngine
{
    /* Enable shared from this 可以让this ptr变成shared_ptr的形式提出*/
    /* 假设我们不用他，强行把this转换成shared_ptr,计数会乱掉*/
    /* https://kheresy.wordpress.com/2018/08/08/enable_shared_from_this/ */
	class DescriptorAllocatorPage : public std::enable_shared_from_this<DescriptorAllocatorPage>
    {
    public:
        DescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);

        D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType() const;

        /* Check if enough space left */
        bool HasSpace(uint32_t numDescriptors) const;
        /* Get number of free handle */
        uint32_t NumFreeHandles() const;

        DescriptorAllocation Allocate(uint32_t numDescriptors);
        void Free(DescriptorAllocation && descriptorHandle, uint64_t frameNumber);
        void ReleaseStaleDescriptors(uint64_t frameNumber);
    protected:
        // Compute the offset of the descriptor handle from the start of the heap.
        uint32_t ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle);

        // Adds a new block to the free list.
        void AddNewBlock(uint32_t offset, uint32_t numDescriptors);

        // Free a block of descriptors.
        // This will also merge free blocks in the free list to form larger blocks
        // that can be reused.
        void FreeBlock(uint32_t offset, uint32_t numDescriptors);
	};

}

