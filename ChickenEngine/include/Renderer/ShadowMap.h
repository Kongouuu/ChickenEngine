#pragma once
#include "Helper/DX12CommonHeader.h"
#include "Helper/Singleton.h"
#include "Device.h"
#include "DescriptorHeap.h"
#include "CommandList.h"
#include "PSO.h"
#include "UploadBuffer.h"

namespace ChickenEngine
{
	class ShadowMap : public Singleton<ShadowMap>
	{
	public:
		static void Init(UINT width, UINT height);
		static void OnResize(UINT width, UINT height);
		static void BuildDescriptors();
		static void BuildResource();
		static void BeginShadowMap(UINT passCBByteSize, Microsoft::WRL::ComPtr<ID3D12Resource> passCB);
		static void EndShadowMap();
		
	private:
		UINT mHeapIndex;
		D3D12_VIEWPORT mViewport;
		D3D12_RECT mScissorRect;
		UINT mWidth = 0;
		UINT mHeight = 0;
		DXGI_FORMAT mFormat = DXGI_FORMAT_R24G8_TYPELESS;

		Microsoft::WRL::ComPtr<ID3D12Resource> mShadowMap = nullptr;
	};
}


