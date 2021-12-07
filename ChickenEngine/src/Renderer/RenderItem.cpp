#include "pch.h"
#include "Renderer\RenderItem.h"


namespace ChickenEngine
{
    int RenderItemManager::renderItemCount = 0;

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
        return POS_NORM_TEX;
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

    std::shared_ptr<RenderItem> RenderItemManager::GetRenderItem( UINT id)
    {
        LOG_INFO("RenderItemManager - Get render item. id: {0}", id);
        if (GetInstance().mIdToItem.find(id) == GetInstance().mIdToItem.end())
        {
            assert(false);
        }
        std::pair< ERenderItemType, UINT> riIndex = GetInstance().mIdToItem[id];
        return GetInstance().mRenderItems[riIndex.first][riIndex.second];
    }

    std::shared_ptr<RenderItem> RenderItemManager::CreateRenderItem(std::string name, ERenderItemType riType)
    {
        LOG_INFO("RenderItemManager - Create render item. name:", name);
        RenderItemManager& rim = GetInstance();
        std::shared_ptr<RenderItem> ri = std::make_shared<RenderItem>();

        // If name in used, add a number behind
        //name = rim.ValidifyName(name);

        // Get current render item count;
        ri->renderItemID = renderItemCount;
        ri->name = name;
        ri->riType = riType;

        rim.mIdToItem[renderItemCount] = std::pair<ERenderItemType, UINT>(riType, rim.mRenderItems[riType].size());
        rim.mRenderItems[riType].emplace_back(ri);
        renderItemCount++;
        return ri;
    }

    void RenderItemManager::InitRenderItem(const Mesh& mesh, UINT id)
    {
        std::pair< ERenderItemType, UINT> riIndex = GetInstance().mIdToItem[id];
        std::shared_ptr<RenderItem> ri = GetInstance().mRenderItems[riIndex.first][riIndex.second];
        InitRenderItem(mesh, ri);
    }


    void RenderItemManager::InitRenderItem(const Mesh& mesh, ERenderItemType riType, UINT index)
    {
        std::shared_ptr<RenderItem> ri = GetInstance().mRenderItems[riType][index];
        InitRenderItem(mesh, ri);
    }

    void RenderItemManager::InitRenderItem(const Mesh& mesh, std::shared_ptr<RenderItem> ri)
    {
        LOG_INFO("RenderItemManager - Init render item. name:", ri->name);
        ri->mesh = mesh;

        
        // vertex buffer
        std::vector<VertexPNT> vertices(mesh.vertices.size());
        for (size_t i = 0; i < mesh.vertices.size(); ++i)
        {
            vertices[i].pos = mesh.vertices[i].pos;
            vertices[i].normal = mesh.vertices[i].normal;
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
