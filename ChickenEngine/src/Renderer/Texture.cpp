#include "pch.h"
#include "Renderer/Texture.h"


namespace ChickenEngine
{
    UINT TextureManager::textureCount = 0;

    TextureManager& TextureManager::GetInstance()
    {
        static TextureManager instance;
        return instance;
    }

    void TextureManager::Init(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList)
    {
        LOG_INFO("TextureManager - Init");
        GetInstance().md3dDevice = d3dDevice;
        GetInstance().mCmdList = cmdList;
    }

    std::shared_ptr<Texture> TextureManager::GetTexture(std::string name)
    {
        LOG_INFO("TextureManager - Get texture. name: {0}", name);
        TextureManager& tm = GetInstance();
        // find in 2d
        if (tm.mTextureMap2D.find(name) != tm.mTextureMap2D.end())
            return tm.mTextureMap2D[name];
        // find in 3d
        if (tm.mTextureMap3D.find(name) != tm.mTextureMap3D.end())
            return tm.mTextureMap3D[name];

        LOG_ERROR("Texture not found");
        assert(false);
        return nullptr;
    }

    std::shared_ptr<Texture> TextureManager::GetTexture(UINT id)
    {
        return GetTexture(GetInstance().mIdNameMap[id]);
    }

    int TextureManager::LoadTexture(std::wstring file, std::string texName, ETextureType textureType)
    {
        LOG_INFO("TextureManager - Load texture. name: {0}", texName);
        TextureManager& tm = GetInstance();
        
        // Check if name is used
        tm.CheckNameValidity(texName);
        // Check if file exist
        std::filesystem::path filePath(file);
        if (!std::filesystem::exists(filePath))
        {
            throw std::exception("File not found.");
        }

        // no lock, and didnt check if loaded be4
        auto tex = std::make_shared<Texture>();
        tex->Name = texName;
        tex->Filename = filePath.filename();


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
        if(textureType == TEXTURE2D)
            tm.mTextureMap2D[tex->Name] = tex;
        else
            tm.mTextureMap3D[tex->Name] = tex;
        tm.mIdNameMap[tex->id] = tex->Name;

        DescriptorHeapManager::BuildTextureSrvHeap(textureType, tex->id, tex->Resource);
    }


    TextureManager::TextureManager()
    {
    }

    TextureManager::~TextureManager()
    {
    }

    void TextureManager::LoadTextureFromWIC(std::wstring fileName, Microsoft::WRL::ComPtr<ID3D12Resource>& texture, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadHeap)
    {
        LOG_INFO("TextureManager - Load texture from wic. path: {0}", fileName);
        std::unique_ptr<uint8_t[]> data;
        D3D12_SUBRESOURCE_DATA subresource;

        ThrowIfFailed(DirectX::LoadWICTextureFromFile(md3dDevice.Get(), fileName.c_str(), texture.GetAddressOf(), data, subresource));

        const UINT64 uploadBufferSize = GetRequiredIntermediateSize(texture.Get(), 0, 1);

        auto heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto buffer = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
        ThrowIfFailed(md3dDevice->CreateCommittedResource(
            &heapProperty, 
            D3D12_HEAP_FLAG_NONE, 
            &buffer,
            D3D12_RESOURCE_STATE_GENERIC_READ, 
            nullptr, 
            IID_PPV_ARGS(uploadHeap.GetAddressOf())
        ));


        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
        mCmdList->ResourceBarrier(1, &barrier);

        UpdateSubresources(mCmdList.Get(), texture.Get(), uploadHeap.Get(), 0, 0, 1, &subresource);

        barrier = CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        mCmdList->ResourceBarrier(1, &barrier);
    }

    void TextureManager::LoadTextureFromDDS(std::wstring fileName, Microsoft::WRL::ComPtr<ID3D12Resource>& texture, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadHeap)
    {

    }

    void TextureManager::CheckNameValidity(std::string name)
    {
        if (mTextureMap2D.find(name) != mTextureMap2D.end())
        {
            if (mTextureMap3D.find(name) != mTextureMap3D.end())
            {
                LOG_ERROR("Texture-Manager - Texture name already taken");
                assert(false);
            }
        }
    }
}