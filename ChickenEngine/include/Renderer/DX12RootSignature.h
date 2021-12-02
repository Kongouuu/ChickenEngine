#pragma once

#include "DX12CommonHeader.h"

namespace ChickenEngine
{
	class CHICKEN_API DX12RootSignature
	{
	public:

		static void Init(int numTextures, Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice);
		static Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature(std::string name);

	private:
		DX12RootSignature();
		~DX12RootSignature();

		static DX12RootSignature& GetInstance();
		void LoadRootSignatures(int numTextures);
		std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers();
	private:
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12RootSignature>> mRootSignatures;
	};

}

