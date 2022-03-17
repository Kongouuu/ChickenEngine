#include "pch.h"
#include "Renderer\DX12Renderer\SkyBox.h"


namespace ChickenEngine
{
	void SkyBox::BindSkyTex(std::shared_ptr<DX12Texture> tex)
	{
		instance().mSrcSkyTex = tex;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE SkyBox::GetSkyTexHandle()
	{
		return instance().mSrcSkyTex->handle;
	}
}

