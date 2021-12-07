#pragma once
#include "pch.h"

#include "Engine/Core.h"
#include <Engine/Events/Event.h>
#include "DX12CommonHeader.h"
#include "DescriptorHeap.h"
#include "Shader.h"
#include "Buffer.h"

namespace ChickenEngine
{

	class CHICKEN_API DX12Renderer
	{
		using EventCallbackFn = std::function<void(Event&)>;
		friend class Application;
	public:
		DX12Renderer();
		~DX12Renderer();
		static DX12Renderer& GetInstance();

		bool Get4xMsaaState()const;
		void Set4xMsaaState(bool value);

	public:
		inline Microsoft::WRL::ComPtr<ID3D12Device> D3dDevice() const { return md3dDevice; }
		inline Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SrvHeap() const { return mSrvHeap; }
		inline Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList() const { return mCommandList; }

	protected:
		// Init DX12
		bool InitDX12(HWND hwnd, int width, int height);
		void CreateCommandObjects();
		void CreateSwapChain();
		void CreateSrvRtvDsvDescriptorHeaps();
		void FlushCommandQueue();
		void OnResize(int width, int height);

		// Init pipeline
		void InitSubsystem();
		void InitShaders();

		// Render
		void PrepareCommandList();
		void CloseCommandList();

		inline ID3D12Resource* CurrentBackBuffer()const { return mSwapChainBuffer[mCurrBackBuffer].Get(); }
		inline D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const { return CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrBackBuffer, mRtvDescriptorSize); }
		inline D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const { return mDsvHeap->GetCPUDescriptorHandleForHeapStart(); }
	private:
		HWND      mhMainWnd = nullptr; // main window handle
		int mWidth;
		int mHeight;

		bool      m4xMsaaState = false;    // 4X MSAA enabled
		UINT      m4xMsaaQuality = 0;      // quality level of 4X MSAA

		Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;

		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
		UINT64 mCurrentFence = 0;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

		static const int SwapChainBufferCount = 2;
		int mCurrBackBuffer = 0;
		Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
		Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

		D3D12_VIEWPORT mScreenViewport;
		D3D12_RECT mScissorRect;

		UINT mRtvDescriptorSize = 0;
		UINT mDsvDescriptorSize = 0;
		UINT mCbvSrvUavDescriptorSize = 0;

		// Derived class should set these in derived constructor to customize starting values.
		std::wstring mMainWndCaption = L"d3d App";
		D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
		DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	};
}


