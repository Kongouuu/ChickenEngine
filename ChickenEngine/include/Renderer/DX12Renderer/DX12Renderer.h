#pragma once
#include "pch.h"

#include "Helper/DX12CommonHeader.h"
#include "Helper/FileHelper.h"
#include "Interface/IResource.h"
#include "RootSignature.h"
#include "Texture.h"
#include "PSO.h"
#include "DescriptorHeap.h"
#include "Shader.h"
#include "Buffer.h"
#include "RenderItem.h"
#include "FrameResource.h"
#include "ShadowMap.h"

namespace ChickenEngine
{

	class CHICKEN_API DX12Renderer
	{

	public:
		DX12Renderer();
		~DX12Renderer();
		static DX12Renderer& GetInstance();

		bool Get4xMsaaState()const;
		void Set4xMsaaState(bool value);

	public:
		inline Microsoft::WRL::ComPtr<ID3D12Device> D3dDevice() const { return md3dDevice; }
		inline Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SrvHeap() const { return DescriptorHeapManager::SrvHeap(); }
		inline Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList() const { return mCmdList; }

	public:
		// Init DX12
		bool InitDX12(HWND hwnd, int width, int height, int numFrameResources = 3);
		void CreateCommandObjects();
		void CreateSwapChain();
		void FlushCommandQueue();
		
		// Input Assembly ( Called by Engine)
		UINT LoadTexture2D(std::string fileName);
		UINT LoadTexture3D(std::string fileName);
		UINT CreateRenderItem(UINT vertexCount, size_t vertexSize, BYTE* vertexData, std::vector<uint16_t> indices, UINT cbOffset);
		UINT CreateDebugRenderItem(UINT vertexCount, size_t vertexSize, BYTE* vertexData, std::vector<uint16_t> indices);

		// Init pipeline
		void CreateFrameResources();
		void InitPipeline();
		
		// Update {every loop}
		void UpdateFrame();
		void SwapFrameResource();
		void UpdatePassCB();
		void UpdateObjectCB();

		// Called update
		void SetPassSceneData(BYTE* data);
		void SetObjectCB(UINT objCBIndex, BYTE* data);
		void SetVisibility(UINT renderItemID, bool visibility);
		void SetDiffuseTexture(UINT renderItemID, UINT offset);
		void SetSpecularTexture(UINT renderItemID, UINT offset);
		void SetNormalTexture(UINT renderItemID, UINT offset);
		void SetHeightTexture(UINT renderItemID, UINT offset);
		void OnResize(int width, int height);

		void SetEnableShadowPass(bool enable);

		// Render
		void BindObjectCB(UINT id);
		void BindAllMapToNull();
		void BindMap(UINT slot, UINT id);
		void Render();
		void RenderDefault();
		void RenderAllItems();
		void RenderRenderItem(std::shared_ptr<RenderItem> ri);
		void EndRender();

		// Other
		void StartDirectCommands();
		void ExecuteCommands();


		inline ID3D12Resource* CurrentBackBuffer()const 
		{ 
			return mSwapChainBuffer[mCurrBackBuffer].Get(); 
		}

		inline D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const 
		{ 
			return CD3DX12_CPU_DESCRIPTOR_HANDLE(DescriptorHeapManager::RtvHeap()->GetCPUDescriptorHandleForHeapStart(), mCurrBackBuffer, DescriptorHeapManager::RtvDescriptorSize()); 
		}

		inline D3D12_GPU_DESCRIPTOR_HANDLE CurrentBackBufferGPUHandle()const
		{
			return CD3DX12_GPU_DESCRIPTOR_HANDLE(DescriptorHeapManager::RtvHeap()->GetGPUDescriptorHandleForHeapStart(), mCurrBackBuffer, DescriptorHeapManager::RtvDescriptorSize());
		}

		inline D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const 
		{ 
			return DescriptorHeapManager::DsvHeap()->GetCPUDescriptorHandleForHeapStart(); 
		}

	private:
		HWND      mhMainWnd = nullptr; // main window handle
		int		  mWidth;
		int		  mHeight;

		bool      m4xMsaaState = false;    // 4X MSAA enabled
		UINT      m4xMsaaQuality = 0;      // quality level of 4X MSAA


		std::vector<std::shared_ptr<FrameResource>> mFrameResources;
		FrameResource* mCurrFrameResource = nullptr;
		int mCurrFrameResourceIndex = 0;
		int mNumFrameResources = 3;

		Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;

		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
		UINT64 mCurrentFence = 0;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCmdQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCmdList;

		static const int SwapChainBufferCount = 2;
		int mCurrBackBuffer = 0;
		Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
		Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;


		D3D12_VIEWPORT mScreenViewport;
		D3D12_RECT mScissorRect;

		// Derived class should set these in derived constructor to customize starting values.
		D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
		DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		UINT mPassCBByteSize;
		UINT mObjectCBByteSize;
		std::vector<BYTE> mPassCBData;

		UINT mObjectCBCount = 0;
		std::unordered_map<UINT, std::vector<BYTE>> mObjectCBData;
		std::unordered_map<UINT, UINT> mObjectCBFramesDirty;
		
		std::vector<bool> mRenderQueue;
		UINT mTextureCount;

		// Special passes
		bool bEnableShadowPass = true;
		std::shared_ptr<RenderItem> debugItem;
	};
}


