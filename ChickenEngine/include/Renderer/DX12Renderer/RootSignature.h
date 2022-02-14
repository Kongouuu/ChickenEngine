#pragma once

#include "Helper/DX12CommonHeader.h"

namespace ChickenEngine
{
	enum ETextureSlot
	{
		SLOT_SKY = 3,
		SLOT_SHADOW = 4,
		SLOT_DIFFUSE = 5,
		SLOT_SPECULAR = 6,
		SLOT_NORMAL = 7,
		SLOT_HEIGHT = 8
	};

	class CHICKEN_API RootSignatureManager : public Singleton<RootSignatureManager>
	{
	public:

		static void Init();
		static Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetRootSignature(std::string name);

	private:
		void LoadRootSignatures();
		void CreateRootSignatureSimple(std::string name, int numCBV, int numSRV);
		std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers();
	private:
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12RootSignature>> mRootSignatures;
	};

}

