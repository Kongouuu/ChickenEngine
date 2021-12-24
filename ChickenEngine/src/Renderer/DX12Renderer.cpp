#include "pch.h"
#include "Renderer/DX12Renderer.h"

namespace ChickenEngine
{
	using namespace DirectX;
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
	bool DX12Renderer::InitDX12(HWND hwnd, int width, int height, int numFrameRersources)
	{
		mhMainWnd = hwnd;
		mWidth = width;
		mHeight = height;
		mNumFrameResources = numFrameRersources;

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
		Device::SetDevice(md3dDevice);

		ThrowIfFailed(md3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&mFence)));

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


		return true;
	}

	void DX12Renderer::CreateCommandObjects()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		ThrowIfFailed(md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCmdQueue)));

		ThrowIfFailed(md3dDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(mDirectCmdListAlloc.GetAddressOf())));

		ThrowIfFailed(md3dDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			mDirectCmdListAlloc.Get(), // Associated command allocator
			nullptr,                   // Initial PipelineStateObject
			IID_PPV_ARGS(mCmdList.GetAddressOf())));
		CommandList::SetCmdList(mCmdList);
		// Start off in a closed state.  This is because the first time we refer 
		// to the command list we will Reset it, and it needs to be closed before
		// calling Reset.
		mCmdList->Close();
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
			mCmdQueue.Get(),
			&sd,
			mSwapChain.GetAddressOf());
	}

	void DX12Renderer::FlushCommandQueue()
	{
		mCurrentFence++;
		ThrowIfFailed(mCmdQueue->Signal(mFence.Get(), mCurrentFence));

		// Wait until the GPU has completed commands up to this fence point.
		if (mFence->GetCompletedValue() < mCurrentFence)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

			// Fire event when GPU hits current fence.  
			ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));

			// Wait until the GPU hits current fence event is fired.
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	void DX12Renderer::OnResize(int width, int height)
	{
		mWidth = width;
		mHeight = height;

		if (md3dDevice == nullptr)
		{
			return;
		}
		LOG_INFO("OnResize");
		assert(md3dDevice);
		assert(mSwapChain);
		assert(mDirectCmdListAlloc);

		// Flush before changing any resources.
		FlushCommandQueue();

		ThrowIfFailed(mCmdList->Reset(mDirectCmdListAlloc.Get(), nullptr));

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

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(DescriptorHeapManager::RtvHeap()->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < SwapChainBufferCount; i++)
		{
			mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i]));
			md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
			rtvHeapHandle.Offset(1, DescriptorHeapManager::RtvDescriptorSize());
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

		// Transition the resource from its initial state to be used as a depth buffer.
		mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

		// Execute the resize commands.
		ThrowIfFailed(mCmdList->Close());
		ID3D12CommandList* cmdsLists[] = { mCmdList.Get() };
		mCmdQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

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

		float aspectRatio = static_cast<float>(mWidth) / mHeight;
		LOG_INFO("reset camera, new fov: {0}", aspectRatio);

		XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, float(mWidth)/float(mHeight), 1.0f, 1000.0f);
	}


#pragma endregion InitDX12

#pragma region InitPipeline
	/*****************************************************************/
	/********************** Pre Input Assembly ***********************/
	/*****************************************************************/
	void DX12Renderer::PreInputAssembly()
	{
		DescriptorHeapManager::InitDescriptorHeapManager(SwapChainBufferCount);
	}

	void DX12Renderer::SetPassCBByteSize(UINT size)
	{
		mPassCBByteSize = size;
	}

	void DX12Renderer::SetObjectCBByteSize(UINT size)
	{
		mObjectCBByteSize = size;
	}

	void DX12Renderer::SetMaterialCBByteSize(UINT size)
	{
		mMaterialCBByteSize = size;
	}


	/*****************************************************************/
	/*********************** Input Assembly **************************/
	/*****************************************************************/
	UINT DX12Renderer::LoadTexture2D(std::string fileName)
	{
		wchar_t tmp[256] = L"\0";
		setlocale(LC_ALL, "chs");
		MultiByteToWideChar(CP_ACP, 0, fileName.c_str(), fileName.length(), tmp, fileName.length());
		std::wstring wFilePath;
		wFilePath.assign(tmp);
		//std::wstring wFilePath(fileName.begin(), fileName.end());
		UINT id = TextureManager::LoadTexture(wFilePath, ETextureDimension::TEXTURE2D);
		
		return id;
	}

	UINT DX12Renderer::LoadTexture3D(std::string fileName)
	{
		std::wstring wFilePath(fileName.begin(), fileName.end());
		UINT id = TextureManager::LoadTexture(wFilePath, ETextureDimension::TEXTURE3D);

		return id;
	}

	UINT DX12Renderer::AddObjectCB()
	{
		UINT id = mObjectCBCount++;
		return id;
	}

	UINT DX12Renderer::AddMaterialCB()
	{
		UINT id = mMaterialCBCount++;
		return id;
	}

	UINT DX12Renderer::CreateRenderItem(UINT vertexCount, size_t vertexSize, BYTE* vertexData, std::vector<uint16_t> indices)
	{
		auto ri = RenderItemManager::CreateRenderItem(RI_OPAQUE);
		ri->Init(vertexCount, vertexSize, vertexData, indices);
		ri->numFramesDirty = mNumFrameResources;
		return ri->renderItemID;
	}



	/*****************************************************************/
	/*********************** Init Pipeline ***************************/
	/*****************************************************************/
	void DX12Renderer::InitPipeline()
	{
		LOG_TRACE("Init pipeline");

		// Build Root Sinagture
		RootSignatureManager::Init();

		// Build descriptor heaps
		DescriptorHeapManager::BuildRtvSrvDsvHeapDesc(TextureManager::TextureCount());
		DescriptorHeapManager::BuildCommonSrvHeap();
		TextureManager::InitTextureHeaps();

		// Init shader
		ShaderManager::Init();

		// build pso;
		PSOManager::Init(mBackBufferFormat, mDepthStencilFormat, m4xMsaaState, m4xMsaaQuality);
		PSOManager::BuildPSOs();

		
		ExecuteCommands();
		FlushCommandQueue();
	
	}

	void DX12Renderer::CreateFrameResources(int numFrames)
	{
		mNumFrameResources = numFrames;
		for (int i = 0; i < mNumFrameResources; ++i)
		{

			mFrameResources.push_back(std::make_shared<FrameResource>(
				1, mObjectCBCount, mMaterialCBCount, mPassCBByteSize, mObjectCBByteSize, mMaterialCBByteSize));
		}
	}

#pragma endregion InitPipeline

#pragma region Pipeline Update
	void DX12Renderer::UpdateFrame()
	{
		SwapFrameResource();
		UpdatePassCB();
		UpdateObjectCB();
		UpdateMaterialCB();
	}

	void DX12Renderer::SwapFrameResource()
	{
		mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % mNumFrameResources;
		mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

		// We have 3 frame resources, currently CPU is on the (n)th frame
		// Check if gpu finished the (n-3)th frame
		if (mCurrFrameResource->Fence != 0 && mFence->GetCompletedValue() < mCurrFrameResource->Fence)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
			ThrowIfFailed(mFence->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	void DX12Renderer::UpdatePassCB()
	{
		mCurrFrameResource->PassCB->CopyData(0, mPassCBData.data());
	}

	void DX12Renderer::UpdateObjectCB()
	{
		if (mCurrFrameResource == nullptr)
			return;

		auto currObjectCB = mCurrFrameResource->ObjectCB.get();
		for (int i = 0; i < mObjectCBCount; i++)
		{
			if (mObjectCBFramesDirty[i])
			{
				currObjectCB->CopyData(i, mObjectCBData[i].data());
				mObjectCBFramesDirty[i]--;
			}
		}
		
	}

	void DX12Renderer::UpdateMaterialCB()
	{
		if (mCurrFrameResource == nullptr)
			return;

		auto currMaterialCB = mCurrFrameResource->MaterialCB.get();
		for (int i = 0; i <mMaterialCBCount; i++)
		{
			if (mMaterialCBFramesDirty[i])
			{
				currMaterialCB->CopyData(i, mMaterialCBData[i].data());
				mMaterialCBFramesDirty[i]--;
			}
		}
	}

	void DX12Renderer::SetPassSceneData(BYTE* data)
	{
		mPassCBData = std::vector<BYTE>(data, data + mPassCBByteSize);
	}

	void DX12Renderer::SetObjectCB(UINT objCBIndex, BYTE* data)//XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale)
	{
		mObjectCBData[objCBIndex] = std::vector(data, data + mObjectCBByteSize);
		mObjectCBFramesDirty[objCBIndex] = mNumFrameResources;
	}

	void DX12Renderer::SetMaterialCB(UINT matCBIndex, BYTE* data)// float roughness, float metallic, XMFLOAT4 color)
	{
		mMaterialCBData[matCBIndex] = std::vector(data, data + mMaterialCBByteSize);
		mMaterialCBFramesDirty[matCBIndex] = mNumFrameResources;
	}

	//void DX12Renderer::SetRenderItemTexture(UINT renderItemID, UINT textureID)
	//{
	//	std::shared_ptr<RenderItem> ri = RenderItemManager::GetRenderItem(renderItemID);
	//	if (ri != nullptr)
	//	{
	//		ri->texOffset = textureID + DescriptorHeapManager::TextureSrvOffset();;
	//		LOG_TRACE("texid set : {0}", ri->texOffset);
	//	}
	//}



#pragma endregion Pipeline Update

#pragma region Pipeline Draw
	void DX12Renderer::BindObjectCB(UINT id)
	{
		auto objectCBByteSize = UploadBufferUtil::CalcConstantBufferByteSize(mObjectCBByteSize);
		auto objectCB = mCurrFrameResource->ObjectCB->Resource();
		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + id * objectCBByteSize;
		mCmdList->SetGraphicsRootConstantBufferView(0, objCBAddress);
	}

	void DX12Renderer::BindMaterialCB(UINT id)
	{
		auto materialCBByteSize = UploadBufferUtil::CalcConstantBufferByteSize(mMaterialCBByteSize);
		auto matCB = mCurrFrameResource->MaterialCB->Resource();
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + id * materialCBByteSize;
		mCmdList->SetGraphicsRootConstantBufferView(1, matCBAddress);
	}

	void DX12Renderer::BindAllMapToNull()
	{
		mCmdList->SetGraphicsRootDescriptorTable(3, DescriptorHeapManager::NullCubeSrv());
		mCmdList->SetGraphicsRootDescriptorTable(4, DescriptorHeapManager::NullTexSrv());
		mCmdList->SetGraphicsRootDescriptorTable(5, DescriptorHeapManager::NullTexSrv());
		mCmdList->SetGraphicsRootDescriptorTable(6, DescriptorHeapManager::NullTexSrv());
		mCmdList->SetGraphicsRootDescriptorTable(7, DescriptorHeapManager::NullTexSrv());
		mCmdList->SetGraphicsRootDescriptorTable(8, DescriptorHeapManager::NullTexSrv());
	}

	/* 这里也可以考虑用单个函数+enum，不过个人不想让renderer这里和engine那共用enum */
	/* 之后的开发中如果能从engine侧去定义root signature,就可以删掉这些函数了 */
	void DX12Renderer::BindDiffuseMap(UINT id)
	{
		BindMap(id, 5);
	}

	void DX12Renderer::BindSpecularMap(UINT id)
	{
		BindMap(id, 6);
	}

	void DX12Renderer::BindNormalMap(UINT id)
	{
		BindMap(id, 7);
	}

	void DX12Renderer::BindHeightMap(UINT id)
	{
		BindMap(id, 8);
	}

	void DX12Renderer::BindMap(UINT id, UINT slot)
	{
		if (id >= 0)
		{
			CD3DX12_GPU_DESCRIPTOR_HANDLE tex(DescriptorHeapManager::SrvHeap()->GetGPUDescriptorHandleForHeapStart());
			tex.Offset(id + DescriptorHeapManager::TextureSrvOffset(), DescriptorHeapManager::CbvSrvUavDescriptorSize());
			mCmdList->SetGraphicsRootDescriptorTable(slot, tex);
		}
	}

	void DX12Renderer::PrepareDraw()
	{
		assert(mCmdList);

		auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;
		assert(cmdListAlloc);
		// Reuse the memory associated with command recording.
		// We can only reset when the associated command lists have finished execution on the GPU.
		ThrowIfFailed(cmdListAlloc->Reset());

		// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
		// Reusing the command list reuses memory.
		ThrowIfFailed(mCmdList->Reset(cmdListAlloc.Get(), PSOManager::GetPSO("default").Get()));

		// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
		mCmdList->RSSetViewports(1, &mScreenViewport);
		mCmdList->RSSetScissorRects(1, &mScissorRect);

		// Indicate a state transition on the resource usage.
		mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		// Clear the back buffer and depth buffer.
		mCmdList->ClearRenderTargetView(CurrentBackBufferView(), DirectX::Colors::LightSteelBlue, 0, nullptr);
		mCmdList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		// Specify the buffers we are going to render to.
		mCmdList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

		ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapManager::SrvHeap().Get() };
		mCmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		mCmdList->SetGraphicsRootSignature(RootSignatureManager::GetRootSignature("default").Get());

		auto passCB = mCurrFrameResource->PassCB->Resource();
		mCmdList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());
	}

	void DX12Renderer::DrawRenderItem(UINT renderItemID)
	{
		std::shared_ptr<RenderItem>& ri = RenderItemManager::GetRenderItem(renderItemID);
		mCmdList->IASetVertexBuffers(0, 1, &ri->vb.VertexBufferView());
		mCmdList->IASetIndexBuffer(&ri->ib.IndexBufferView());
		mCmdList->IASetPrimitiveTopology(ri->PrimitiveType);
		mCmdList->DrawIndexedInstanced(ri->indexCount, 1, 0, 0, 0);
	}


	void DX12Renderer::EndDraw()
	{
		// Indicate a state transition on the resource usage.
		mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		ExecuteCommands();
		// swap the back and front buffers
		ThrowIfFailed(mSwapChain->Present(0, 0));
		mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

		mCurrFrameResource->Fence = ++mCurrentFence;

		mCmdQueue->Signal(mFence.Get(), mCurrentFence);
	}

	void DX12Renderer::StartDirectCommands()
	{
		// Reset the command list to prep for initialization commands.
		ThrowIfFailed(mCmdList->Reset(mDirectCmdListAlloc.Get(), nullptr));
	}

	void DX12Renderer::ExecuteCommands()
	{
		// Done recording commands.
		ThrowIfFailed(mCmdList->Close());

		// Add the command list to the queue for execution.
		ID3D12CommandList* cmdsLists[] = { mCmdList.Get() };
		mCmdQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	}
#pragma endregion Pipeline



}