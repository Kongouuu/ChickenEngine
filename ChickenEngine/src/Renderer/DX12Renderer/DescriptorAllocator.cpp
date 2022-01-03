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

        // ������availableHeaps��Allocate n��������
        for (auto iter = mAvailableHeaps.begin(); iter != mAvailableHeaps.end(); ++iter)
        {
            auto allocatorPage = mHeapPool[*iter];

            // �ȳ��������page�����
            allocation = allocatorPage->Allocate(numDescriptors);
            // ����������page���ˣ�������Ƴ�available heap
            if (allocatorPage->NumFreeHandles() == 0)
            {
                iter = mAvailableHeaps.erase(iter);
            }

            // �������ɹ����˳�
            if (!allocation.IsNull())
            {
                break;
            }
        }
        
        // �������available heap�������㣬��ʾû�з���ɹ������½�һ��page
        if (allocation.IsNull())
        {
            // �·����ʱ�򣬸�����������Ҫ���ÿ��heap������������
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