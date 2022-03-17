#pragma once
#include "Helper/DX12CommonHeader.h"
#include "DescriptorHeap.h"
#include "PSO.h"
#include "Texture.h"
namespace ChickenEngine
{
	class SkyBox : public Singleton<SkyBox>
	{
	public:
		static void BindSkyTex(std::shared_ptr<DX12Texture> tex);
		static D3D12_GPU_DESCRIPTOR_HANDLE GetSkyTexHandle();
	private:
		std::shared_ptr<DX12Texture> mSrcSkyTex;
	};

}

