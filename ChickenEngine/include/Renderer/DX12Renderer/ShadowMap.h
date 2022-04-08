#pragma once
#include "Helper/DX12CommonHeader.h"
#include "DescriptorHeap.h"
#include "PSO.h"
#include "UploadBuffer.h"
#include "FrameBuffer.h"
#include "ComputeBuffer.h"

namespace ChickenEngine
{
	class ShadowMap : public Singleton<ShadowMap>
	{
	public:
		static void Init(uint32_t width, uint32_t height);
		static void OnResize(uint32_t width, uint32_t height);
		static void BeginShadowMap(uint32_t passCBByteSize, Microsoft::WRL::ComPtr<ID3D12Resource> passCB);
		static void EndShadowMap();

		static void GenerateVSMMipMap();
		inline static D3D12_GPU_DESCRIPTOR_HANDLE SrvGpuHandle() { return instance().mSrvGpuHandle; }
		inline static D3D12_GPU_DESCRIPTOR_HANDLE SrvGpuHandleVSM() { return instance().mVsmBuffer.GetSrvHandle(); }

		bool bEnableVSM = false;
	protected:
		void BuildDescriptors();
		void BuildResource();
		
	private:
		uint32_t mHeapIndex;
		D3D12_VIEWPORT mViewport;
		D3D12_RECT mScissorRect;
		uint32_t mWidth = 0;
		uint32_t mHeight = 0;
		DXGI_FORMAT mFormat = DXGI_FORMAT_R24G8_TYPELESS;
		DXGI_FORMAT mMomentFormat = DXGI_FORMAT_R32G32_FLOAT;

		int mSrvOffset = -1;
		int mDsvOffset = -1;
		D3D12_GPU_DESCRIPTOR_HANDLE mSrvGpuHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE mDsvCpuHandle;
		
		FrameBuffer mSquaredShadowMap;

		Microsoft::WRL::ComPtr<ID3D12Resource> mShadowMap = nullptr;
		ComputeBuffer mVsmBuffer;
	};
}


