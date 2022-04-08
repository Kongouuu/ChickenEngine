#pragma once
#include "Helper/DX12CommonHeader.h"
#include "DescriptorHeap.h"
#include "PSO.h"
#include "UploadBuffer.h"
#include "FrameBuffer.h"
#include "ComputeBuffer.h"


namespace ChickenEngine
{
	class GBuffer : Singleton<GBuffer>
	{
	public:
		static void Init();
		static void BuildResource(int width, int height);
		static void BeginGBufferRender(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);
		static void EndGBufferRender();
		static inline D3D12_GPU_DESCRIPTOR_HANDLE AlbedoHandle() { return instance().mAlbedoBuffer.GetSrvHandle(); }
		static inline D3D12_GPU_DESCRIPTOR_HANDLE PositionHandle() { return instance().mPositionBuffer.GetSrvHandle(); }
		static inline D3D12_GPU_DESCRIPTOR_HANDLE NormalHandle() { return instance().mNormalBuffer.GetSrvHandle(); }
	private:
		D3D12_VIEWPORT mViewport;
		D3D12_RECT mScissorRect;
		uint32_t mWidth = 0;
		uint32_t mHeight = 0;

		D3D12_CPU_DESCRIPTOR_HANDLE mHandles[3];
		// albedo + roughness
		FrameBuffer mAlbedoBuffer;
		// world position, w is projected space's w
		FrameBuffer mPositionBuffer;
		// normal + metallic
		FrameBuffer mNormalBuffer;
	};
}


