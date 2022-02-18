#include "pch.h"
#include "Renderer/DX12Renderer/Texture.h"


namespace ChickenEngine
{
    uint32_t TextureManager::textureCount = 0;

    void TextureManager::Init()
    {
        instance().LoadNullTextures();
    }

    void TextureManager::LoadNullTextures()
    {
        std::shared_ptr<DX12Texture> nullTex2d = std::make_shared<DX12Texture>();
        nullTex2d->id = 0;
        nullTex2d->Resource = nullptr;
        nullTex2d->TextureDimension = ETextureDimension::TEXTURE2D;
        nullTex2d->offset = DescriptorHeapManager::BindSrv(nullptr, ETextureDimension::TEXTURE2D);
        nullTex2d->handle = DescriptorHeapManager::GetSrvUavGpuHandle(nullTex2d->offset);
        mTextures.push_back(nullTex2d);

        std::shared_ptr<DX12Texture> nullTex3d = std::make_shared<DX12Texture>();
        nullTex3d->id = 1;
        nullTex3d->Resource = nullptr;
        nullTex3d->TextureDimension = ETextureDimension::TEXTURE3D;
        nullTex3d->offset = DescriptorHeapManager::BindSrv(nullptr, ETextureDimension::TEXTURE3D);
        nullTex3d->handle = DescriptorHeapManager::GetSrvUavGpuHandle(nullTex3d->offset);
        mTextures.push_back(nullTex3d);
        textureCount = 2;
    }

    std::shared_ptr<DX12Texture> TextureManager::GetTexture(uint32_t id)
    {
        return instance().mTextures[id];
    }

    uint32_t TextureManager::LoadTexture(std::wstring file, ETextureDimension textureDimension)
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
        std::shared_ptr<DX12Texture> tex = std::make_shared<DX12Texture>();
        tex->Filename = filePath.filename();
        tex->TextureDimension = textureDimension;

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
        tex->offset = DescriptorHeapManager::BindSrv(tex->Resource.Get(), tex->TextureDimension);
        tex->handle = DescriptorHeapManager::GetSrvUavGpuHandle(tex->offset);
        instance().mTextures.push_back(tex);

        return tex->id;
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