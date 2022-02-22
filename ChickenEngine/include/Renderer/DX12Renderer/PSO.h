#pragma once

#include "Helper/DX12CommonHeader.h"

#include "Shader.h"
#include "RootSignature.h"
#include "InputLayout.h"

namespace ChickenEngine
{
	class CHICKEN_API PSOManager : public Singleton<PSOManager>
	{
	public:
		static void Init(DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat, bool msaaState, uint32_t msaaQuality);
		static void BuildPSOs();
		static Microsoft::WRL::ComPtr<ID3D12PipelineState>& GetPSO(std::string name);

		static void UsePSO(std::string name);
	protected:
		static const D3D12_GRAPHICS_PIPELINE_STATE_DESC& DefaultPsoDesc();
		inline static const D3D12_SHADER_BYTECODE VSByteCode(std::string name)
		{
			return  { reinterpret_cast<BYTE*>(ShaderManager::GetVS(name)->GetBufferPointer()), ShaderManager::GetVS(name)->GetBufferSize() };
		}
		inline static const D3D12_SHADER_BYTECODE PSByteCode(std::string name)
		{
			return { reinterpret_cast<BYTE*>(ShaderManager::GetPS(name)->GetBufferPointer()), ShaderManager::GetPS(name)->GetBufferSize() };
		}

	private:
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mPSOs;
		DXGI_FORMAT mBackBufferFormat;
		DXGI_FORMAT mDepthStencilFormat; 
		bool m4xMsaaState;
		uint32_t m4xMsaaQuality;
	};

}

