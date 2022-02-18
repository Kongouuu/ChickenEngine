#pragma once

#include "Helper/DX12CommonHeader.h"
#include "Interface/IResource.h"
#include "Device.h"

namespace ChickenEngine
{
	class DescriptorHeap
	{
	public:
		void Create(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors, bool bShaderVisible = false);
		inline Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Get() { return mHeap; }

		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(uint32_t offset)
		{
			CD3DX12_GPU_DESCRIPTOR_HANDLE handle(mGpuHeapStart, offset, mIncrementSize);
			return handle;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32_t offset)
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle(mCpuHeapStart, offset, mIncrementSize);
			return handle;
		}

	private:
		D3D12_DESCRIPTOR_HEAP_DESC mHeapDesc;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mHeap;
		D3D12_GPU_DESCRIPTOR_HANDLE mGpuHeapStart;
		D3D12_CPU_DESCRIPTOR_HANDLE mCpuHeapStart;
		uint32_t mIncrementSize;
	};

	class DescriptorHeapManager : public Singleton<DescriptorHeapManager>
	{
	public:
		inline static void InitDescriptorHeapManager(int swapChainBufferCount) { instance().Init(swapChainBufferCount); }
		inline static void BuildRtvSrvDsvHeapDesc() { instance().BuildDesc(); }
		inline static D3D12_GPU_DESCRIPTOR_HANDLE GetSrvUavGpuHandle(uint32_t offset) { return instance().mSrvUavHeap.GetGpuHandle(offset); }
		inline static D3D12_CPU_DESCRIPTOR_HANDLE GetSrvUavCpuHandle(uint32_t offset) { return instance().mSrvUavHeap.GetCpuHandle(offset); }
		inline static D3D12_CPU_DESCRIPTOR_HANDLE GetRtvCpuHandle(uint32_t offset) { return instance().mRtvHeap.GetCpuHandle(offset); }
		inline static D3D12_GPU_DESCRIPTOR_HANDLE GetRtvGpuHandle(uint32_t offset) { return instance().mRtvHeap.GetGpuHandle(offset); }
		inline static D3D12_CPU_DESCRIPTOR_HANDLE GetDsvCpuHandle(uint32_t offset) { return instance().mDsvHeap.GetCpuHandle(offset); }

		// Bind resource with a descriptor heap
		static uint32_t BindSrv(ID3D12Resource* resource, ETextureDimension dimension = ETextureDimension::TEXTURE2D);
		static uint32_t BindSrv(ID3D12Resource* resource, ETextureDimension dimension, DXGI_FORMAT format, UINT16 mipLevels = 1);
		static uint32_t BindRtv(ID3D12Resource* resource, bool bBackBuffer = false, uint32_t backBufferID = 0);
		static uint32_t BindDsv(ID3D12Resource* resource, bool bBackBuffer = false);
		static uint32_t BindUav(ID3D12Resource* resource, size_t mipSlice, ETextureDimension dimension = ETextureDimension::TEXTURE2D);
		static uint32_t BindUav(ID3D12Resource* resource, size_t mipSlice, ETextureDimension dimension, DXGI_FORMAT format, UINT16 mipLevels = 1);

		// Rebind resource with offset;
		static bool RebindSrv(ID3D12Resource* resource, uint32_t offset);
		static bool RebindRtv(ID3D12Resource* resource, uint32_t offset);
		static bool RebindDsv(ID3D12Resource* resource, uint32_t offset);
		static bool RebindUav(ID3D12Resource* resource, uint32_t offset);

		// Unbind resource
		static void UnbindSrv(uint32_t offset);
		static void UnbindRtv(uint32_t offset);
		static void UnbindDsv(uint32_t offset);
		static void UnbindUav(uint32_t offset);

		inline static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSrvHeap() { return instance().mSrvUavHeap.Get();  }
	public:
		void Init(int swapChainBufferCount);
		void BuildDesc();
	private:
		int mSwapChainBufferCount = 0;
		const size_t srvuavSize = 65535;
		const size_t rtvSize = 256;
		const size_t dsvSize = 256;

		DescriptorHeap mSrvUavHeap;
		DescriptorHeap mRtvHeap;
		DescriptorHeap mDsvHeap;

		std::vector<uint32_t> mSrvUavAvailable;
		std::vector<uint32_t> mRtvAvailable;
		std::vector<uint32_t> mDsvAvailable;

		std::unordered_map<uint32_t, D3D12_SHADER_RESOURCE_VIEW_DESC> mSrvDescByOffset;
		std::unordered_map<uint32_t, D3D12_RENDER_TARGET_VIEW_DESC> mRtvDescByOffset;
		std::unordered_map<uint32_t, D3D12_DEPTH_STENCIL_VIEW_DESC> mDsvDescByOffset;
		std::unordered_map<uint32_t, D3D12_UNORDERED_ACCESS_VIEW_DESC> mUavDescByOffset;

	};
}
