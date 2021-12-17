#pragma once

#include "Helper/DX12CommonHeader.h"
#include "DescriptorHeap.h"
#include "WICTextureLoader/WICTextureLoader12.h"

namespace ChickenEngine
{
	struct DX12Texture
	{
		UINT id;
		// Unique material name for lookup.
		std::string Name;

		std::wstring Filename;

		Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
	};

	class CHICKEN_API TextureManager : public Singleton<TextureManager>
	{

	public:
		static std::shared_ptr<DX12Texture> GetTexture(UINT id);
		static std::shared_ptr<DX12Texture> GetTexture(std::string name);
		static int LoadTexture(std::wstring file, std::string texName, ETextureType textureType);
		

		static inline UINT TextureCount() { return textureCount; }
		

	protected:

		void LoadTextureFromWIC(std::wstring fileName, Microsoft::WRL::ComPtr<ID3D12Resource>& texture, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadHeap);
		void LoadTextureFromDDS(std::wstring fileName, Microsoft::WRL::ComPtr<ID3D12Resource>& texture, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadHeap);

		void CheckNameValidity(std::string name);
	private:
		std::unordered_map<std::string, std::shared_ptr<DX12Texture>> mTextureMap2D;
		std::unordered_map<std::string, std::shared_ptr<DX12Texture>> mTextureMap3D;
		std::unordered_map<UINT, std::string> mIdNameMap;

		static UINT textureCount;
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

