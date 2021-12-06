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

    void TextureManager::Init(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, Microsoft::WRL::ComPtr<ID3D12CommandList> cmdList)
    {
        GetInstance().md3dDevice = d3dDevice;
        GetInstance().mCmdList = cmdList;
    }

    std::shared_ptr<Texture> TextureManager::GetTexture(std::string name)
    {
        return GetInstance().mTextureMap[name];
    }

    std::shared_ptr<Texture> TextureManager::GetTexture(UINT id)
    {
        return GetTexture(GetInstance().mIdNameMap[id]);
    }

    int TextureManager::LoadTexture(std::wstring file, std::string texName)
    {
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
            GetInstance().LoadTextureFromWIC(file, tex->Resource, tex->UploadHeap);
        }

        tex->id = textureCount;
        textureCount++;

        GetInstance().mTextureMap[tex->Name] = tex;
        GetInstance().mIdNameMap[tex->id] = tex->Name;
    }


    TextureManager::TextureManager()
    {
    }
    TextureManager::~TextureManager()
    {
    }

    void TextureManager::LoadTextureFromWIC(std::wstring fileName, Microsoft::WRL::ComPtr<ID3D12Resource>& texture, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadHeap)
    {
        std::unique_ptr<UINT8_t[]> data;
        D3D12_SUBRESOURCE_DATA subresource;

        ThrowIfFailed(DirectX::LoadWICTextureFromFile(md3dDevice.Get(), fileName.c_str(), texture.GetAddressOf(), data, subresource));

        const UINT64 uploadBufferSize = GetRequiredIntermediateSize(texture.Get(), 0, 1);

        ThrowIfFailed(md3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 
            D3D12_HEAP_FLAG_NONE, 
            &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ, 
            nullptr, 
            IID_PPV_ARGS(uploadHeap.GetAddressOf())
        ));


        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
        mCmdList->ResourceBarrier(1, &barrier);

        UpdateSubresources(mCmdList.Get(), texture.Get(), uploadHeap.Get(), 0, 0, 1, &subresource);

        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        mCmdList->ResourceBarrier(1, &barrier);
    }

    void TextureManager::LoadTextureFromDDS(std::wstring fileName, Microsoft::WRL::ComPtr<ID3D12Resource>& texture, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadHeap)
    {

    }
}