#pragma once

#include "Helper/DX12CommonHeader.h"
#include "DescriptorHeap.h"
#include "WICTextureLoader/WICTextureLoader12.h"
#include "Interface/IResource.h"

namespace ChickenEngine
{
	struct DX12Texture
	{
		uint32_t id;
		uint32_t offset;
		// Unique material name for lookup.
		std::wstring Filename;
		ETextureDimension TextureDimension;
		D3D12_GPU_DESCRIPTOR_HANDLE handle;
		Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
	};

	class CHICKEN_API TextureManager : public Singleton<TextureManager>
	{
	public:
		static void Init();
		static std::shared_ptr<DX12Texture> GetTexture(uint32_t id);
		static uint32_t LoadTexture(std::wstring file, ETextureDimension textureDimension);
		inline static uint32_t TextureCount() { return textureCount; }

		inline static D3D12_GPU_DESCRIPTOR_HANDLE NullTex2DHandle() { return instance().mTextures[0]->handle; }
		inline static D3D12_GPU_DESCRIPTOR_HANDLE NullTex3DHandle() { return instance().mTextures[1]->handle; }

	protected:
		void LoadNullTextures();
		void LoadTextureFromWIC(std::wstring fileName, Microsoft::WRL::ComPtr<ID3D12Resource>& texture, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadHeap);
		void LoadTextureFromDDS(std::wstring fileName, Microsoft::WRL::ComPtr<ID3D12Resource>& texture, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadHeap);

		//void CheckNameValidity(std::string name);
	private:
		std::vector<std::shared_ptr<DX12Texture>> mTextures;

		static uint32_t textureCount;
	};


	enum DDS_ALPHA_MODE
	{
		DDS_ALPHA_MODE_UNKNOWN = 0,
		DDS_ALPHA_MODE_STRAIGHT = 1,
		DDS_ALPHA_MODE_PREMULTIPLIED = 2,
		DDS_ALPHA_MODE_OPAQUE = 3,
		DDS_ALPHA_MODE_CUSTOM = 4,
	};


}

