#pragma once

#include "Helper/DX12CommonHeader.h"

namespace ChickenEngine
{
	class CHICKEN_API DescriptorHeapManager : public Singleton<DescriptorHeapManager>
	{
	public:
		static void InitDescriptorHeapManager(int swapChainBufferCount);
		static void BuildRtvSrvDsvHeapDesc(int numTex);
		static void BuildCommonSrvHeap();
		static void BuildShadowMapHeap(Microsoft::WRL::ComPtr<ID3D12Resource> shadowMap);
		static void BuildTextureSrvHeap(ETextureDimension texType, UINT offset, Microsoft::WRL::ComPtr<ID3D12Resource> resource);\

	public:
		inline static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SrvHeap(){ return instance().mSrvHeap; }
		inline static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RtvHeap(){ return instance().mRtvHeap; }
		inline static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DsvHeap(){ return instance().mDsvHeap; }

		inline static CD3DX12_GPU_DESCRIPTOR_HANDLE NullTexSrv() { return instance().mNullTexSrv; }
		inline static CD3DX12_GPU_DESCRIPTOR_HANDLE NullCubeSrv() { return instance().mNullCubeSrv; }

		inline static CD3DX12_CPU_DESCRIPTOR_HANDLE ShadowDsv() { return CD3DX12_CPU_DESCRIPTOR_HANDLE(DsvHeap()->GetCPUDescriptorHandleForHeapStart(), 1, instance().mDsvDescriptorSize); }

		inline static UINT ImguiSrvOffset() { return instance().mImguiSrvOffset; }
		inline static UINT NullTexSrvOffset() { return instance().mNullTexSrvOffset; }
		inline static UINT NullCubeSrvOffset() { return instance().mNullCubeSrvOffset; }
		inline static UINT ShadowSrvOffset() { return instance().mShadowSrvOffset; }
		inline static UINT TextureSrvOffset() { return instance().mTextureSrvOffset; }

		inline static UINT RtvDescriptorSize() { return instance().mRtvDescriptorSize; }
		inline static UINT DsvDescriptorSize() { return instance().mDsvDescriptorSize; }
		inline static UINT CbvSrvUavDescriptorSize() { return instance().mCbvSrvUavDescriptorSize; }

	private:
		int mSwapChainBufferCount = 0;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

		CD3DX12_GPU_DESCRIPTOR_HANDLE mNullTexSrv;
		CD3DX12_GPU_DESCRIPTOR_HANDLE mNullCubeSrv;

		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;

		UINT mImguiSrvOffset;
		UINT mNullTexSrvOffset;
		UINT mNullCubeSrvOffset;
		UINT mShadowSrvOffset;
		UINT mTextureSrvOffset;

	};
}


