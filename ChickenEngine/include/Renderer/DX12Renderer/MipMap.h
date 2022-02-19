#pragma once
#include "Helper/DX12CommonHeader.h"
#include "Helper/Singleton.h"
#include "RootSignature.h"
#include "Shader.h"
#include "Device.h"
#include "CommandList.h"
#include "FrameBuffer.h"
#include "UploadBuffer.h"
#include "PSO.h"
#include "DescriptorHeap.h"

namespace ChickenEngine
{

	class MipMapManager : public Singleton<MipMapManager>
	{
	public:
		void Init();
		void InitShader();
		void InitRootsignature();
		void InitPSO();
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>>& GetMipPSO() { return mipPSO; }
	private:

		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mipPSO;
		bool bInit = false;
	};


}
