#include "pch.h"
#include "Renderer/DX12Renderer.h"
#include <Engine/Log.h>
#include <DirectXColors.h>
#include "Renderer/d3dx12.h"

namespace ChickenEngine
{
	DX12Renderer::DX12Renderer()
	{
	}

	DX12Renderer::~DX12Renderer()
	{
	}

	DX12Renderer& DX12Renderer::GetInstance()
	{
		static DX12Renderer instance;
		return instance;
	}
#pragma region Settings
	bool DX12Renderer::Get4xMsaaState() const
	{
		return m4xMsaaState;
	}

	void DX12Renderer::Set4xMsaaState(bool value)
	{
		if (m4xMsaaState != value)
		{
			m4xMsaaState = value;

			// Recreate the swapchain and buffers with new multisample settings.
			CreateSwapChain();
			OnResize(mWidth, mHeight);
		}
	}
#pragma endregion Settings

#pragma region InitDX12
	bool DX12Renderer::InitDX12(HWND hwnd, int width, int height)
	{
		mhMainWnd = hwnd;
		mWidth = width;
		mHeight = height;

		if (hwnd == NULL)
		{
			LOG_ERROR("nonexisting hwnd");
			return false;
		}
		CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory));

		// Try to create hardware device.
		HRESULT hardwareResult = D3D12CreateDevice(
			nullptr,             // default adapter
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&md3dDevice));
		// Fallback to WARP device.
		if (FAILED(hardwareResult))
		{
			LOG_ERROR("Failed hardware result");
			Microsoft::WRL::ComPtr<IDXGIAdapter> pWarpAdapter;
			mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter));

			D3D12CreateDevice(
				pWarpAdapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&md3dDevice));
		}

		ThrowIfFailed(md3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&mFence)));

		mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// Check 4X MSAA quality support for our back buffer format.
		// All Direct3D 11 capable devices support 4X MSAA for all render 
		// target formats, so we only need to check quality support.

		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
		msQualityLevels.Format = mBackBufferFormat;
		msQualityLevels.SampleCount = 4;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		ThrowIfFailed(md3dDevice->CheckFeatureSupport(
			D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
			&msQualityLevels,
			sizeof(msQualityLevels)));

		m4xMsaaQuality = msQualityLevels.NumQualityLevels;
		assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");


		CreateCommandObjects();
		CreateSwapChain();
		CreateSrvRtvDsvDescriptorHeaps();

		return true;
	}

	void DX12Renderer::CreateCommandObjects()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		ThrowIfFailed(md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

		ThrowIfFailed(md3dDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(mDirectCmdListAlloc.GetAddressOf())));

		ThrowIfFailed(md3dDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			mDirectCmdListAlloc.Get(), // Associated command allocator
			nullptr,                   // Initial PipelineStateObject
			IID_PPV_ARGS(mCommandList.GetAddressOf())));

		// Start off in a closed state.  This is because the first time we refer 
		// to the command list we will Reset it, and it needs to be closed before
		// calling Reset.
		mCommandList->Close();
	}

	void DX12Renderer::CreateSwapChain()
	{
		LOG_INFO("Create swapchain {0} {1}", mWidth, mHeight);
		// Release the previous swapchain we will be recreating.
		mSwapChain.Reset();

		DXGI_SWAP_CHAIN_DESC sd;
		sd.BufferDesc.Width = mWidth;
		sd.BufferDesc.Height = mHeight;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.Format = mBackBufferFormat;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.SampleDesc.Count = m4xMsaaState ? 4 : 1;
		sd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = SwapChainBufferCount;
		sd.OutputWindow = mhMainWnd;
		sd.Windowed = true;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// Note: Swap chain uses queue to perform flush.
		mdxgiFactory->CreateSwapChain(
			mCommandQueue.Get(),
			&sd,
			mSwapChain.GetAddressOf());
	}

	void DX12Renderer::CreateSrvRtvDsvDescriptorHeaps()
	{
		D3D12_DESCRIPTOR_HEAP_DESC SrvHeapDesc;
		SrvHeapDesc.NumDescriptors = 1;
		SrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		SrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		SrvHeapDesc.NodeMask = 0;
		ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
			&SrvHeapDesc, IID_PPV_ARGS(mSrvHeap.GetAddressOf())));

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;
		ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
			&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));


		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDesc.NodeMask = 0;
		ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
			&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
	}

	void DX12Renderer::FlushCommandQueue()
	{
		mCurrentFence++;
		mCommandQueue->Signal(mFence.Get(), mCurrentFence);

		// Wait until the GPU has completed commands up to this fence point.
		if (mFence->GetCompletedValue() < mCurrentFence)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

			// Fire event when GPU hits current fence.  
			mFence->SetEventOnCompletion(mCurrentFence, eventHandle);

			// Wait until the GPU hits current fence event is fired.
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	void DX12Renderer::OnResize(int width, int height)
	{
		mWidth = width;
		mHeight = height;
		if (md3dDevice == NULL)
		{
			LOG_ERROR("null device");
			return;
		}
			
		LOG_INFO("OnResize");
		assert(md3dDevice);
		assert(mSwapChain);
		assert(mDirectCmdListAlloc);

		// Flush before changing any resources.
		FlushCommandQueue();

		mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr);

		// Release the previous resources we will be recreating.
		for (int i = 0; i < SwapChainBufferCount; ++i)
			mSwapChainBuffer[i].Reset();
		mDepthStencilBuffer.Reset();

		// Resize the swap chain.
		LOG_INFO("Resize width {0} height {1}", width, height);
		mSwapChain->ResizeBuffers(
			SwapChainBufferCount,
			width, height,
			mBackBufferFormat,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		mCurrBackBuffer = 0;

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < SwapChainBufferCount; i++)
		{
			mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i]));
			md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
			rtvHeapHandle.Offset(1, mRtvDescriptorSize);
		}

		// Create the depth/stencil buffer and view.
		D3D12_RESOURCE_DESC depthStencilDesc;
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = width;
		depthStencilDesc.Height = height;
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.Format = mDepthStencilFormat;
		depthStencilDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
		depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = mDepthStencilFormat;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;
		CD3DX12_HEAP_PROPERTIES HeapDefault(D3D12_HEAP_TYPE_DEFAULT);
		ThrowIfFailed(md3dDevice->CreateCommittedResource(
			&HeapDefault,
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));

		// Create descriptor to mip level 0 of entire resource using the format of the resource.
		md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), nullptr, DepthStencilView());


		CD3DX12_RESOURCE_BARRIER Transition = CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		// Transition the resource from its initial state to be used as a depth buffer.
		mCommandList->ResourceBarrier(1, &Transition);

		// Execute the resize commands.
		mCommandList->Close();
		ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// Wait until resize is complete.
		FlushCommandQueue();

		// Update the viewport transform to cover the client area.
		mScreenViewport.TopLeftX = 0;
		mScreenViewport.TopLeftY = 0;
		mScreenViewport.Width = static_cast<float>(width);
		mScreenViewport.Height = static_cast<float>(height);
		mScreenViewport.MinDepth = 0.0f;
		mScreenViewport.MaxDepth = 1.0f;

		mScissorRect = { 0, 0, (long)width, (long)height };
	}


#pragma endregion InitDX12

#pragma region InitPipeline
	void DX12Renderer::InitSubsystem()
	{
		BufferManager::InitBufferManager(md3dDevice, mCommandList);
	}

	void DX12Renderer::InitShaders()
	{
		Shader::Init();
	}
#pragma endregion InitPipeline

#pragma region Pipeline
	void DX12Renderer::PrepareCommandList()
	{
		assert(mCommandList);
		assert(mDirectCmdListAlloc);
		// Reuse the memory associated with command recording.
// We can only reset when the associated command lists have finished execution on the GPU.
		mDirectCmdListAlloc->Reset();

		// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
		// Reusing the command list reuses memory.
		mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr);

		// Indicate a state transition on the resource usage.
		CD3DX12_RESOURCE_BARRIER Transition = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		mCommandList->ResourceBarrier(1, &Transition);

		// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
		mCommandList->RSSetViewports(1, &mScreenViewport);
		mCommandList->RSSetScissorRects(1, &mScissorRect);

		// Clear the back buffer and depth buffer.
		mCommandList->ClearRenderTargetView(CurrentBackBufferView(), DirectX::Colors::LightSteelBlue, 0, nullptr);
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		// Specify the buffers we are going to render to.
		D3D12_CPU_DESCRIPTOR_HANDLE CurBackBufferView = CurrentBackBufferView();
		D3D12_CPU_DESCRIPTOR_HANDLE CurDepthStencilView = DepthStencilView();
		mCommandList->OMSetRenderTargets(1, &CurBackBufferView, true, &CurDepthStencilView);

		// Indicate a state transition on the resource usage.
		mCommandList->ResourceBarrier(1, &Transition);
	}

	void DX12Renderer::CloseCommandList()
	{
		assert(mCommandList);
		assert(mSwapChain);
		// Done recording commands.
		mCommandList->Close();

		// Add the command list to the queue for execution.
		ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// swap the back and front buffers
		mSwapChain->Present(0, 0);
		mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

		// Wait until frame commands are complete.  This waiting is inefficient and is
		// done for simplicity.  Later we will show how to organize our rendering code
		// so we do not have to wait per frame.
	}
#pragma endregion Pipeline
}