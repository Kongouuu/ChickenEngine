#include "pch.h"
#include "Renderer\RenderItem.h"


namespace ChickenEngine
{
    /* Render Item */
    EVertexLayout RenderItem::GetLayoutType()
    {
        //switch (riType)
        //{
        //case RI_OPAQUE:
        //    return POS_NORM_COL_TEX;
        //case RI_TRANSPARENT:
        //    return POS_NORM_COL_TEX;
        //default:
        //}
        return POS_NORM_COL_TEX;
    }

    /* Render Item Manager*/
    RenderItemManager& RenderItemManager::GetInstance()
    {
        static RenderItemManager instance;
        return instance;
    }

    //void RenderItemManager::InitRenderItemManager(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice)
    //{
    //    GetInstance().md3dDevice = d3dDevice;
    //}

    std::shared_ptr<RenderItem> RenderItemManager::GetRenderItem(ERenderItemType riType, UINT id)
    {
        return GetInstance().mRenderItems[riType][id];
    }

    std::shared_ptr<RenderItem> RenderItemManager::CreateRenderItem(std::string name, ERenderItemType riType)
    {
        RenderItemManager& rim = GetInstance();
        std::shared_ptr<RenderItem> ri = std::make_shared<RenderItem>();

        // If name in used, add a number behind
        //name = rim.ValidifyName(name);

        // Get current render item count;
        UINT riCount = GetInstance().mRenderItems.size();
        ri->id = riCount;
        ri->name = name;
        ri->riType = riType;

        rim.mRenderItems[riType].emplace_back(ri);
        return ri;
    }

    void RenderItemManager::InitRenderItem(const Mesh& mesh, ERenderItemType riType, UINT id)
    {
        std::shared_ptr<RenderItem> ri = GetInstance().mRenderItems[riType][id];
        InitRenderItem(mesh, ri);
    }

    void RenderItemManager::InitRenderItem(const Mesh& mesh, std::shared_ptr<RenderItem> ri)
    {
        ri->mesh = mesh;

        
        // vertex buffer
        std::vector<VertexPNCT> vertices(mesh.vertices.size());
        for (size_t i = 0; i < mesh.vertices.size(); ++i)
        {
            vertices[i].pos = mesh.vertices[i].pos;
            vertices[i].normal = mesh.vertices[i].normal;
            vertices[i].color = mesh.vertices[i].color;
            vertices[i].texC = mesh.vertices[i].texC;
        }
        //index buffer
        std::vector<UINT> indices(mesh.indices);


        const UINT vbByteSize = (UINT)vertices.size() * sizeof(VertexPNCT);
        const UINT ibByteSize = (UINT)indices.size() * sizeof(UINT);

        ThrowIfFailed(D3DCreateBlob(vbByteSize, &ri->vb.VertexBufferCPU));
        CopyMemory(ri->vb.VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

        ThrowIfFailed(D3DCreateBlob(ibByteSize, &ri->ib.IndexBufferCPU));
        CopyMemory(ri->ib.IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

        ri->vb.VertexBufferGPU = BufferManager::CreateDefaultBuffer(vertices.data(), vbByteSize, ri->vb.VertexBufferUploader);
        ri->ib.IndexBufferGPU = BufferManager::CreateDefaultBuffer(indices.data(), ibByteSize, ri->ib.IndexBufferUploader);


        ri->vb.VertexByteStride = sizeof(Vertex);
        ri->vb.VertexBufferByteSize = vbByteSize;
        ri->ib.IndexFormat = DXGI_FORMAT_R16_UINT;
        ri->ib.IndexBufferByteSize = ibByteSize;
    }

    //std::string RenderItemManager::ValidifyName(std::string name)
    //{
    //    RenderItemManager& rim = GetInstance();
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
