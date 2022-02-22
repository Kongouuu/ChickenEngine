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

	class BlurFilterManager : public Singleton<BlurFilterManager>
	{
	public:
		static std::vector<float> CalcGaussWeight(float sigma);
		void Init();
		void InitShader();
		void InitRootsignature();
		void InitPSO();
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>>& GetBlurPSO() { return blurPSO; }
	private:
		const int MaxBlurRadius = 5;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> blurPSO;
		bool bInit = false;
	};


}
