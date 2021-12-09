#include "pch.h"
#include "Engine/Scene/RenderItem.h"
#include "Engine/Log.h"

namespace ChickenEngine
{
    void RenderItem::Init(const Mesh& _mesh)
    {
        LOG_INFO("RenderItem - Init render item. name: {0}", name.c_str());
        mesh = _mesh;


        LOG_INFO("Mesh vertices size £º {0}", mesh.vertices.size());
        // vertex buffer
        std::vector<VertexPNT> vertices(mesh.vertices.size());
        for (size_t i = 0; i < mesh.vertices.size(); ++i)
        {
            vertices[i].pos = mesh.vertices[i].pos;
            vertices[i].normal = mesh.vertices[i].normal;
            vertices[i].texC = mesh.vertices[i].texC;
            LOG_INFO("POS_INFO: {0} {1} {2}", vertices[i].pos.x, vertices[i].pos.y, vertices[i].pos.z);
        }
        //index buffer
        std::vector<std::uint16_t> indices(mesh.GetIndices16());

        const UINT vbByteSize = (UINT)vertices.size() * sizeof(VertexPNT);
        const UINT ibByteSize = (UINT)indices.size() * sizeof(uint16_t);

        LOG_ERROR("vbbytesize: {0},      ibbytesize: {1}", vbByteSize, ibByteSize);

        ThrowIfFailed(D3DCreateBlob(vbByteSize, &vb.VertexBufferCPU));
        CopyMemory(vb.VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

        ThrowIfFailed(D3DCreateBlob(ibByteSize, &ib.IndexBufferCPU));
        CopyMemory(ib.IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

        indexCount = (UINT)indices.size();
        vb.VertexBufferGPU = BufferManager::CreateDefaultBuffer(vertices.data(), vbByteSize, vb.VertexBufferUploader);
        ib.IndexBufferGPU = BufferManager::CreateDefaultBuffer(indices.data(), ibByteSize, ib.IndexBufferUploader);



        vb.VertexByteStride = sizeof(VertexPNT);
        vb.VertexBufferByteSize = vbByteSize;
        ib.IndexFormat = DXGI_FORMAT_R16_UINT;
        ib.IndexBufferByteSize = ibByteSize;

        textureHandle = DescriptorHeapManager::NullTexSrv();
    }

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


    //void RenderItemManager::InitRenderItemManager(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice)
    //{
    //    instance().md3dDevice = d3dDevice;
    //}

    std::shared_ptr<RenderItem> RenderItemManager::GetRenderItem( UINT id)
    {
        LOG_INFO("RenderItemManager - Get render item. id: {0}", id);
        if (instance().mIdToItem.find(id) == instance().mIdToItem.end())
        {
            assert(false);
        }
        std::pair< ERenderItemType, UINT> riIndex = instance().mIdToItem[id];
        return instance().mRenderItems[riIndex.first][riIndex.second];
    }

    std::shared_ptr<RenderItem> RenderItemManager::CreateRenderItem(std::string name, ERenderItemType riType)
    {
        LOG_INFO("RenderItemManager - Create render item. name: {0}", name.c_str());
        RenderItemManager& rim = instance();
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
        std::pair< ERenderItemType, UINT> riIndex = instance().mIdToItem[id];
        std::shared_ptr<RenderItem> ri = instance().mRenderItems[riIndex.first][riIndex.second];
        InitRenderItem(mesh, ri);
    }


    void RenderItemManager::InitRenderItem(const Mesh& mesh, ERenderItemType riType, UINT index)
    {
        std::shared_ptr<RenderItem> ri = instance().mRenderItems[riType][index];
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


        const UINT vbByteSize = (UINT)vertices.size() * sizeof(VertexPNT);
        const UINT ibByteSize = (UINT)indices.size() * sizeof(UINT);

        ThrowIfFailed(D3DCreateBlob(vbByteSize, &ri->vb.VertexBufferCPU));
        CopyMemory(ri->vb.VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

        ThrowIfFailed(D3DCreateBlob(ibByteSize, &ri->ib.IndexBufferCPU));
        CopyMemory(ri->ib.IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

        ri->indexCount = (UINT)indices.size();
        ri->vb.VertexBufferGPU = BufferManager::CreateDefaultBuffer(vertices.data(), vbByteSize, ri->vb.VertexBufferUploader);
        ri->ib.IndexBufferGPU = BufferManager::CreateDefaultBuffer(indices.data(), ibByteSize, ri->ib.IndexBufferUploader);


        ri->vb.VertexByteStride = sizeof(VertexPNT);
        ri->vb.VertexBufferByteSize = vbByteSize;
        ri->ib.IndexFormat = DXGI_FORMAT_R16_UINT;
        ri->ib.IndexBufferByteSize = ibByteSize;
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
