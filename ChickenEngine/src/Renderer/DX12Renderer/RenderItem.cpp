#include "pch.h"
#include "Renderer/DX12Renderer/RenderItem.h"

namespace ChickenEngine
{
    void RenderItem::Init(uint32_t vertexCount, size_t vertexSize, BYTE* vertexData, std::vector<uint16_t> indices)
    {
        LOG_INFO("RenderItem - Init render item");

        const uint32_t vbByteSize = (UINT)vertexCount * vertexSize;
        const uint32_t ibByteSize = (UINT)indices.size() * sizeof(uint16_t);

        indexCount = (UINT)indices.size();
        vb.VertexBufferGPU = BufferManager::CreateDefaultBuffer(vertexData, vbByteSize, vb.VertexBufferUploader);
        ib.IndexBufferGPU = BufferManager::CreateDefaultBuffer(indices.data(), ibByteSize, ib.IndexBufferUploader);

        vb.VertexByteStride = vertexSize;
        vb.VertexBufferByteSize = vbByteSize;
        ib.IndexFormat = DXGI_FORMAT_R16_UINT;
        ib.IndexBufferByteSize = ibByteSize;

    }

    void RenderItem::Init()
    {
        LOG_INFO("RenderItem - Init render item");

        const uint32_t vbByteSize = (UINT)4 * sizeof(Vertex);
        const uint32_t ibByteSize = (UINT)6 * sizeof(uint16_t);

        indexCount = 6;
        std::vector<Vertex> vertices = std::vector<Vertex>(4);
        vertices[0] = Vertex({ 0.5, -0.5, 0.0 }, { 0.0,1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0 });
        vertices[1] = Vertex({ 1.0, -0.5, 0.0 }, { 0.0,1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,0.0 });
        vertices[2] = Vertex({ 0.5, -1.0, 0.0 }, { 0.0,1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0 });
        vertices[3] = Vertex({ 1.0, -1.0, -5.0 }, { 0.0,1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,1.0 });
        BYTE* data = reinterpret_cast<BYTE*>(vertices.data());
        vb.VertexBufferGPU = BufferManager::CreateDefaultBuffer(data, vbByteSize, vb.VertexBufferUploader);
        std::vector<uint16_t> i = std::vector<uint16_t>(6);

        i[0] = 0; i[1] = 1; i[2] = 2;
        i[3] = 2; i[4] = 1; i[5] = 3;

        ib.IndexBufferGPU = BufferManager::CreateDefaultBuffer(i.data(), ibByteSize, ib.IndexBufferUploader);

        vb.VertexByteStride = sizeof(Vertex);
        vb.VertexBufferByteSize = vbByteSize;
        ib.IndexFormat = DXGI_FORMAT_R16_UINT;
        ib.IndexBufferByteSize = ibByteSize;
    }

    int RenderItemManager::renderItemCount = 0;


    std::shared_ptr<RenderItem> RenderItemManager::GetRenderItem( uint32_t id)
    {
        return instance().mRenderItems[id];
    }

    std::shared_ptr<RenderItem> RenderItemManager::CreateRenderItem(ERenderLayer riLayer)
    {
        RenderItemManager& rim = instance();
        std::shared_ptr<RenderItem> ri = std::make_shared<RenderItem>();

        // If name in used, add a number behind
        //name = rim.ValidifyName(name);

        // Get current render item count;
        ri->renderItemID = renderItemCount;
        ri->layer = riLayer;
        D3D12_GPU_DESCRIPTOR_HANDLE nullHandle = TextureManager::NullTex2DHandle();
        ri->diffuseHandle = nullHandle;
        ri->specularHandle = nullHandle;
        ri->normalHandle = nullHandle;
        ri->heightHandle = nullHandle;
        rim.mRenderItems.emplace_back(ri);
        rim.mRenderItemOfType[(int)riLayer].emplace_back(ri);
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
