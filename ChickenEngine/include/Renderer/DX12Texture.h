#pragma once

#include "DX12CommonHeader.h"
#include "WICTextureLoader/WICTextureLoader12.h"

namespace ChickenEngine
{
	struct Texture
	{
		uint id;
		// Unique material name for lookup.
		std::string Name;

		std::wstring Filename;

		Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
	};

	class CHICKEN_API DX12TextureManager
	{

	public:
		static DX12TextureManager& GetInstance();
		static void Init(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, Microsoft::WRL::ComPtr<ID3D12CommandList> cmdList);
		static std::shared_ptr<Texture> GetTexture(std::string name);
		static std::shared_ptr<Texture> GetTexture(uint id);
		static int LoadTexture(std::wstring file, std::string texName);

	protected:
		DX12TextureManager();
		virtual ~DX12TextureManager();

		void LoadTextureFromWIC(std::wstring fileName, Microsoft::WRL::ComPtr<ID3D12Resource>& texture, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadHeap);
		void LoadTextureFromDDS(std::wstring fileName, Microsoft::WRL::ComPtr<ID3D12Resource>& texture, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadHeap);

	private:
		std::unordered_map<std::string, std::shared_ptr<Texture>> mTextureMap;
		std::unordered_map<int, std::string> mIdNameMap;
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCmdList;
		static uint textureCount;
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

