#include "pch.h"
#include "Renderer/DX12Renderer/RenderItem.h"

namespace ChickenEngine
{
    void RenderItem::Init(uint32_t vertexCount, size_t vertexSize, BYTE* vertexData, std::vector<uint16_t> indices)
    {
        LOG_INFO("RenderItem - Init render item");

        const uint32_t vbByteSize = (UINT)vertexCount * vertexSize;
        const uint32_t ibByteSize = (UINT)indices.size() * sizeof(uint16_t);

        //ThrowIfFailed(D3DCreateBlob(vbByteSize, &vb.VertexBufferCPU));
        //CopyMemory(vb.VertexBufferCPU->GetBufferPointer(), vertexData, vbByteSize);

        //ThrowIfFailed(D3DCreateBlob(ibByteSize, &ib.IndexBufferCPU));
        //CopyMemory(ib.IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

        indexCount = (UINT)indices.size();
        vb.VertexBufferGPU = BufferManager::CreateDefaultBuffer(vertexData, vbByteSize, vb.VertexBufferUploader);
        ib.IndexBufferGPU = BufferManager::CreateDefaultBuffer(indices.data(), ibByteSize, ib.IndexBufferUploader);

        vb.VertexByteStride = vertexSize;
        vb.VertexBufferByteSize = vbByteSize;
        ib.IndexFormat = DXGI_FORMAT_R16_UINT;
        ib.IndexBufferByteSize = ibByteSize;

        //LOG_ERROR("ID: {0}, vertexCount: {1}, vertexSize: {2}, vbbytesize: {3}, ibbytsize: {4}",
           // renderItemID, vertexCount, vertexSize, vbByteSize, ibByteSize);
    }

    int RenderItemManager::renderItemCount = 0;


    std::shared_ptr<RenderItem> RenderItemManager::GetRenderItem( uint32_t id)
    {
        return instance().mRenderItems[id];
    }

    std::shared_ptr<RenderItem> RenderItemManager::CreateRenderItem(ERenderItemType riType)
    {
        RenderItemManager& rim = instance();
        std::shared_ptr<RenderItem> ri = std::make_shared<RenderItem>();

        // If name in used, add a number behind
        //name = rim.ValidifyName(name);

        // Get current render item count;
        ri->renderItemID = renderItemCount;
        ri->riType = riType;
        D3D12_GPU_DESCRIPTOR_HANDLE nullHandle = TextureManager::NullTex2DHandle();
        ri->diffuseHandle = nullHandle;
        ri->specularHandle = nullHandle;
        ri->normalHandle = nullHandle;
        ri->heightHandle = nullHandle;
        rim.mRenderItems.emplace_back(ri);
       // rim.mRenderItemOfType[(int)riType].emplace_back(ri);
        renderItemCount++;
        return ri;
    }

    //std::string RenderItemManager::ValidifyName(std::string name)
    //{
    //    RenderItemManager& rim = instance();
    //    if (rim.mNameIdMap.find(name) != rim.mNameIdMap.end())
    //    {
    //        std::string tmpname = name;
    //        int num = 1;
    //        while (rim.mNameIdMap.find(tmpname) != rim.mNameIdMap.end())
    //        {
    //            tmpname = name + std::to_string(num);
    //            num++;
    //        }
    //        name = tmpname;
    //    }
    //    return name;
    //}



}
