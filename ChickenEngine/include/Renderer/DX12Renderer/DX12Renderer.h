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
#include "FrameBuffer.h"
#include "MipMap.h"
#include "SkyBox.h"
#include "GBuffer.h"

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
		inline Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SrvHeap() const { return DescriptorHeapManager::GetSrvHeap(); }
		inline Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList() const { return mCmdList; }
		inline int NumFrameResources() const { return  mNumFrameResources; }

	public:
		// Init DX12
		bool InitDX12(HWND hwnd, int width, int height, int numFrameResources = 3);
		void InitSubsystems();
		void CreateCommandObjects();
		void CreateSwapChain();
		void FlushCommandQueue();
		
		// Input Assembly ( Called by Engine)
		uint32_t LoadTexture2D(std::string fileName);
		uint32_t LoadTexture3D(std::string fileName);
		uint32_t BindSkyTex(uint32_t id);
		uint32_t CreateRenderItem(uint32_t vertexCount, size_t vertexSize, BYTE* vertexData, std::vector<uint16_t> indices, ERenderLayer layer);
		uint32_t CreateRenderItem(uint32_t vertexCount, size_t vertexSize, BYTE* vertexData, std::vector<uint16_t> indices, uint32_t cbOffset, ERenderLayer layer);
		uint32_t CreateSkyBoxRenderItem(uint32_t vertexCount, size_t vertexSize, BYTE* vertexData, std::vector<uint16_t> indices, uint32_t cbOffset);
		uint32_t CreateDebugRenderItem(uint32_t vertexCount, size_t vertexSize, BYTE* vertexData, std::vector<uint16_t> indices);

		// Init pipeline
		void CreateFrameResources();
		void InitPipeline();
		
		// Update {every loop}
		void UpdateFrame();
		void SwapFrameResource();
		void UpdatePassCB();
		void UpdateSettingCB();
		void UpdateObjectCB();
		void UpdateRenderSettings(RenderSettings rs);

		// Called update
		void SetPassSceneData(BYTE* data);
		void SetObjectCB(uint32_t objCBIndex, BYTE* data);
		void SetVisibility(uint32_t renderItemID, bool visibility);
		void SetTexture(uint32_t renderItemID, int texID, ETextureType type);
		//
		void OnWindowResize(int width, int height);
		void OnViewportResize(int width, int height);

		// Render
		void BindObjectCB(uint32_t id);
		void BindAllMapToNull();
		void BindMap(uint32_t slot, D3D12_GPU_DESCRIPTOR_HANDLE handle);
		void Render();
		void RenderDefault();
		void RenderSkyBox();
		void RenderAllItems();
		void RenderDebugPlane();
		void EndRender();

		// Other
		void StartDirectCommands();
		void ExecuteCommands();

		inline ID3D12Resource* CurrentBackBuffer() const { return mSwapChainBuffer[mCurrBackBuffer].Get(); }
		inline D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const { return mBackBufferHandleCPU[mCurrBackBuffer]; }
		inline D3D12_GPU_DESCRIPTOR_HANDLE CurrentBackBufferGPUHandle()const { return mBackBufferHandleGPU[mCurrBackBuffer]; }
		inline D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const { return mDepthStencilHandleCPU; }

		inline D3D12_GPU_DESCRIPTOR_HANDLE CurrentViewportBufferGPUHandle() const { return mViewPortBuffer->GetSrvHandle();
	}

	private:
		HWND      mhMainWnd = nullptr; // main window handle
		int		  mWidth;
		int		  mHeight;

		bool      m4xMsaaState = false;    // 4X MSAA enabled
		uint32_t      m4xMsaaQuality = 0;      // quality level of 4X MSAA

		/* frameresource*/
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

		// back buffer
		static const int SwapChainBufferCount = 2;
		int mCurrBackBuffer = 0;
		Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
		Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;
		D3D12_CPU_DESCRIPTOR_HANDLE mBackBufferHandleCPU[SwapChainBufferCount];
		D3D12_GPU_DESCRIPTOR_HANDLE mBackBufferHandleGPU[SwapChainBufferCount];
		D3D12_CPU_DESCRIPTOR_HANDLE mDepthStencilHandleCPU;

		D3D12_VIEWPORT mScreenViewport;
		D3D12_RECT mScissorRect;
		
		// viewport buffer for imgui
		std::shared_ptr<FrameBuffer> mViewPortBuffer;

		// Derived class should set these in derived constructor to customize starting values.
		D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
		DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		uint32_t mPassCBByteSize;
		std::vector<BYTE> mPassCBData;

		RenderSettings mRenderSetting;
		uint32_t mSettingCBByteSize;
		std::vector<BYTE> mSettingCBData;

		uint32_t mObjectCBCount = 0;
		uint32_t mObjectCBByteSize;
		std::unordered_map<UINT, std::vector<BYTE>> mObjectCBData;
		std::unordered_map<UINT, UINT> mObjectCBFramesDirty;
		
		std::vector<bool> mRenderQueue;
		uint32_t mTextureCount;

		// Special passes
		bool bEnableShadowPass = true;
		std::shared_ptr<RenderItem> debugItem;

		std::shared_ptr<DX12Texture> mSkyTex;
	};
}


