#pragma once

#include "DX12CommonHeader.h"

namespace ChickenEngine
{
	class DescriptorHeapManager
	{
	public:
		static DescriptorHeapManager& GetInstance();
		static void InitDescriptorHeapManager(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, int swapChainBufferCount);
		static void BuildRtvSrvDsvHeapDesc(int numTex2D, int numTex3D);
		static void BuildTextureSrvHeap(ETextureType texType, UINT offset, Microsoft::WRL::ComPtr<ID3D12Resource> resource);

	public:
		inline static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SrvHeap(){ return GetInstance().mSrvHeap; }
		inline static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RtvHeap(){ return GetInstance().mRtvHeap; }
		inline static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DsvHeap(){ return GetInstance().mDsvHeap; }

		inline static UINT NullSrvOffset2D() { return 1;}
		inline static UINT NullSrvOffset3D() { return 2;}
		inline static UINT TextureSrvOffsetStart() { return 3; }

		inline static UINT RtvDescriptorSize() { return GetInstance().mRtvDescriptorSize; }
		inline static UINT DsvDescriptorSize() { return GetInstance().mDsvDescriptorSize; }
		inline static UINT CbvSrvUavDescriptorSize() { return GetInstance().mCbvSrvUavDescriptorSize; }

	private:
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
		int mSwapChainBufferCount = 0;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;

	};
}


