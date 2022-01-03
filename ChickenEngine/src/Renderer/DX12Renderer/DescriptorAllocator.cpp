#include "pch.h"
#include "Renderer/DX12Renderer/DescriptorAllocator.h"

namespace ChickenEngine
{
    DescriptorAllocator::DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerHeap)
        : mHeapType(type), mNumDescriptorsPerHeap(numDescriptorsPerHeap)
    {
    }

    DescriptorAllocator::~DescriptorAllocator()
    {
    }

    DescriptorAllocation DescriptorAllocator::Allocate(uint32_t numDescriptors)
    {
        DescriptorAllocation allocation;

        // 尝试在availableHeaps里Allocate n个描述符
        for (auto iter = mAvailableHeaps.begin(); iter != mAvailableHeaps.end(); ++iter)
        {
            auto allocatorPage = mHeapPool[*iter];

            // 先尝试在这个page里分配
            allocation = allocatorPage->Allocate(numDescriptors);
            // 假设分配完后，page满了，则把它移除available heap
            if (allocatorPage->NumFreeHandles() == 0)
            {
                iter = mAvailableHeaps.erase(iter);
            }

            // 假设分配成功，退出
            if (!allocation.IsNull())
            {
                break;
            }
        }
        
        // 如果所有available heap都不满足，表示没有分配成功，就新建一个page
        if (allocation.IsNull())
        {
            // 新分配的时候，根据需求量需要提高每个heap的描述符数量
            mNumDescriptorsPerHeap = max(mNumDescriptorsPerHeap, numDescriptors);
            auto newPage = CreateAllocatorPage();

            allocation = newPage->Allocate(numDescriptors);
        }

        return allocation;
    }

    void DescriptorAllocator::ReleaseStaleDescriptors(uint64_t frameNumber)
    {
        for (size_t i = 0; i < mHeapPool.size(); ++i)
        {
            auto page = mHeapPool[i];

            page->ReleaseStaleDescriptors(frameNumber);

            if (page->NumFreeHandles() > 0)
            {
                mAvailableHeaps.insert(i);
            }
        }
    }

    std::shared_ptr<DescriptorAllocatorPage> DescriptorAllocator::CreateAllocatorPage()
    {
        auto newPage = std::make_shared<DescriptorAllocatorPage>(mHeapType, mNumDescriptorsPerHeap);

        mHeapPool.emplace_back(newPage);
        mAvailableHeaps.insert(mHeapPool.size() - 1);

        return newPage;
    }
};