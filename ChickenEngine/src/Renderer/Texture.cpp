#include "pch.h"
#include "Renderer/Texture.h"


namespace ChickenEngine
{
    UINT TextureManager::textureCount = 0;

    std::shared_ptr<DX12Texture> TextureManager::GetTexture(UINT id)
    {
        return instance().mTextures[id];
    }

    UINT TextureManager::LoadTexture(std::wstring file, ETextureDimension textureType)
    {
        TextureManager& tm = instance();
        LOG_ERROR("enter file load");
        // Check if file exist
        std::filesystem::path filePath(file);

        if (!std::filesystem::exists(filePath))
        {
            LOG_ERROR("file not found");
            throw std::exception("File not found.");
        }

        // no lock, and didnt check if loaded be4
        LOG_ERROR("Before Error in Texture");
        std::shared_ptr<DX12Texture> tex = std::make_shared<DX12Texture>();
        LOG_ERROR("After Error in Texture");
        tex->Filename = filePath.filename();
        tex->TextureType = textureType;

        if (filePath.extension() == ".dds")
        {
            // TODO
        }
        else if (filePath.extension() == ".hdr")
        {
            // TODO
        }
        else
        {
            tm.LoadTextureFromWIC(file, tex->Resource, tex->UploadHeap);
        }

        tex->id = textureCount;
        textureCount++;
        instance().mTextures.push_back(tex);

        return tex->id;
    }

    void TextureManager::InitTextureHeaps()
    {
        for (auto& tex : instance().mTextures)
        {
            DescriptorHeapManager::BuildTextureSrvHeap(tex->TextureType, tex->id, tex->Resource);
        }
       
    }

    void TextureManager::LoadTextureFromWIC(std::wstring fileName, Microsoft::WRL::ComPtr<ID3D12Resource>& texture, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadHeap)
    {
        LOG_INFO("TextureManager - Load texture from wic.");
        std::unique_ptr<uint8_t[]> data;
        D3D12_SUBRESOURCE_DATA subresource;

        ThrowIfFailed(DirectX::LoadWICTextureFromFile(Device::device().Get(), fileName.c_str(), texture.GetAddressOf(), data, subresource));

        const UINT64 uploadBufferSize = GetRequiredIntermediateSize(texture.Get(), 0, 1);

        auto heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto buffer = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
        ThrowIfFailed(Device::device()->CreateCommittedResource(
            &heapProperty, 
            D3D12_HEAP_FLAG_NONE, 
            &buffer,
            D3D12_RESOURCE_STATE_GENERIC_READ, 
            nullptr, 
            IID_PPV_ARGS(uploadHeap.GetAddressOf())
        ));


        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
        CommandList::cmdList()->ResourceBarrier(1, &barrier);

        UpdateSubresources(CommandList::cmdList().Get(), texture.Get(), uploadHeap.Get(), 0, 0, 1, &subresource);

        barrier = CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        CommandList::cmdList()->ResourceBarrier(1, &barrier);
    }

    void TextureManager::LoadTextureFromDDS(std::wstring fileName, Microsoft::WRL::ComPtr<ID3D12Resource>& texture, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadHeap)
    {

    }

 /*   void TextureManager::CheckNameValidity(std::string name)
    {
        if (mTextureMap2D.find(name) != mTextureMap2D.end())
        {
            if (mTextureMap3D.find(name) != mTextureMap3D.end())
            {
                LOG_ERROR("Texture-Manager - Texture name already taken");
                assert(false);
            }
        }
    }*/
}