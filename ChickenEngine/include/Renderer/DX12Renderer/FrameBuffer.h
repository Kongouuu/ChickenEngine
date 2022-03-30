#pragma once
#include "DescriptorHeap.h"
#include "MipMap.h"
#include "Helper/DX12CommonHeader.h"
namespace ChickenEngine
{
	class FrameBuffer
	{
	public:
		void BuildResource(int width, int height, DXGI_FORMAT format, int miplevels = 1);
		void StartRender(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, const FLOAT* colorRGBA = DirectX::Colors::LightSteelBlue);
		void EndRender();
		inline D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandle() { return mBufferSrvGpuHandle; }
		inline D3D12_CPU_DESCRIPTOR_HANDLE GetRtvHandle() { return mBufferRtvCpuHandle; }
		inline Microsoft::WRL::ComPtr<ID3D12Resource> Resource() { return mBuffer; }
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> mBuffer;
		bool bMipmapEnabled = false;
		int mMiplevels = -1;
		int mSrvOffset = -1;
		int mRtvOffset = -1;
		D3D12_GPU_DESCRIPTOR_HANDLE mBufferSrvGpuHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE mBufferRtvCpuHandle;
		D3D12_VIEWPORT mScreenViewport;
		D3D12_RECT mScissorRect;

	};
}


