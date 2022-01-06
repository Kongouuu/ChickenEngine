#pragma once
#include "DescriptorHeap.h"
#include "Helper/DX12CommonHeader.h"
namespace ChickenEngine
{
	class FrameBuffer
	{
	public:
		void BuildResource(int width, int height, DXGI_FORMAT format);
		void StartRender(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);
		void EndRender();
		inline D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandle() { return mBufferSrvGpuHandle; }
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> mBuffer;

		int mSrvOffset = -1;
		int mRtvOffset = -1;
		D3D12_GPU_DESCRIPTOR_HANDLE mBufferSrvGpuHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE mBufferRtvCpuHandle;
		D3D12_VIEWPORT mScreenViewport;
		D3D12_RECT mScissorRect;

	};
}


