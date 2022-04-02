#pragma once

#include "Helper/DX12CommonHeader.h"
#include "DescriptorHeap.h"
#include "PSO.h"
#include "UploadBuffer.h"
#include "FrameBuffer.h"
#include "ComputeBuffer.h"
#include "GBuffer.h"

namespace ChickenEngine 
{

	class SSAO : Singleton<SSAO>
	{
#define SSAO_SAMPLE_NUM 14
		public:
			static void Init(int width, int height);
			static void OnResize(int width, int height);
			static void SSAORender();
			static inline D3D12_GPU_DESCRIPTOR_HANDLE SSAOHandle() { return instance().mSSAOBuffer.GetSrvHandle(); }
		private:
			void BuildRandomVectorTexture();
			void BuildOffsetVectors();
			DirectX::XMFLOAT4 mOffsets[SSAO_SAMPLE_NUM];
			D3D12_VIEWPORT mViewport;
			D3D12_RECT mScissorRect;
			uint32_t mWidth = 0;
			uint32_t mHeight = 0;
			FrameBuffer mSSAOBuffer;


			//random vector generation
			Microsoft::WRL::ComPtr<ID3D12Resource> mRandomVectorMap;
			Microsoft::WRL::ComPtr<ID3D12Resource> mRandomVectorMapUploadBuffer;
			D3D12_GPU_DESCRIPTOR_HANDLE mRandomVectorHandle;
	};
}


