#pragma once

#include "Helper/DX12CommonHeader.h"

namespace ChickenEngine
{
	class CHICKEN_API RootSignatureManager : public Singleton<RootSignatureManager>
	{
	public:

		static void Init();
		static Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetRootSignature(std::string name);

	private:

		void LoadRootSignatures();
		std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers();
	private:
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12RootSignature>> mRootSignatures;
	};

}

