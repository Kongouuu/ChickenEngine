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

		bool GenerateMips(ID3D12Resource* resource, D3D12_GPU_DESCRIPTOR_HANDLE srvHandle);
		bool GenerateMipsf2(ID3D12Resource* resource, D3D12_GPU_DESCRIPTOR_HANDLE srvHandle);
	private:
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> uavResources;
		int curResource = 0;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mipPSO;
		bool bInit = false;
	};


}
