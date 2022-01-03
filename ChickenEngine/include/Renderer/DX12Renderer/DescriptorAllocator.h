#pragma once
#include "pch.h"
#include "Helper/DX12CommonHeader.h"
#include "DescriptorAllocatorPage.h"
#include "DescriptorAllocation.h"

namespace ChickenEngine
{
    class DescriptorAllocator
    {
    public:
        DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerHeap = 256);
        virtual ~DescriptorAllocator();

        DescriptorAllocation Allocate(uint32_t numDescriptors = 1);

        /**
         * When the frame has completed, the stale descriptors can be released.
         */
        void ReleaseStaleDescriptors(uint64_t frameNumber);
    private:
        using DescriptorHeapPool = std::vector< std::shared_ptr<DescriptorAllocatorPage> >;

        // Create a new heap with a specific number of descriptors.
        std::shared_ptr<DescriptorAllocatorPage> CreateAllocatorPage();

        D3D12_DESCRIPTOR_HEAP_TYPE mHeapType;
        uint32_t mNumDescriptorsPerHeap;

        DescriptorHeapPool mHeapPool;
        // Indices of available heaps in the heap pool.
        std::set<size_t> mAvailableHeaps;
    };
}


