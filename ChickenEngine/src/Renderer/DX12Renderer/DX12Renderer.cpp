#include "pch.h"
#include "Renderer/DX12Renderer/DX12Renderer.h"

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
			OnWindowResize(mWidth, mHeight);
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

		// Init constants
		mPassCBByteSize = sizeof(PassConstants);
		mObjectCBByteSize = sizeof(ObjectConstants);
		mSettingCBByteSize = sizeof(RenderSettings);

		for (int i = 0; i < SwapChainBufferCount; i++)
		{
			mViewPortBuffer = std::make_shared<FrameBuffer>();
		}
		
		InitSubsystems();
		return true;
	}

	void DX12Renderer::InitSubsystems()
	{
		// Init subsystems
		DescriptorHeapManager::InitDescriptorHeapManager(SwapChainBufferCount);
		DescriptorHeapManager::BuildRtvSrvDsvHeapDesc();
		TextureManager::Init();
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

	void DX12Renderer::OnWindowResize(int width, int height)
	{
		LOG_INFO("Resize width {0} height {1}", width, height);
		mWidth = width;
		mHeight = height;

		if (md3dDevice == nullptr)
		{
			return;
		}
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
		mSwapChain->ResizeBuffers(
			SwapChainBufferCount,
			width, height,
			mBackBufferFormat,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		mCurrBackBuffer = 0;

		for (uint32_t i = 0; i < SwapChainBufferCount; i++)
		{
			mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i]));
			uint32_t offset = DescriptorHeapManager::BindRtv(mSwapChainBuffer[i].Get(), true, i);
			mBackBufferHandleCPU[i] = DescriptorHeapManager::GetRtvCpuHandle(offset);
			mBackBufferHandleGPU[i] = DescriptorHeapManager::GetSrvGpuHandle(offset);
		}

		//InitViewportBuffer();


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
		ThrowIfFailed(Device::device()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&depthStencilDesc,
			D3D12_RESOURCE_STATE_COMMON,
			&optClear,
			IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));

		LOG_ERROR("After commit dep stencil resource");
		// Create descriptor to mip level 0 of entire resource using the format of the resource.
		DescriptorHeapManager::BindDsv(mDepthStencilBuffer.Get(), true);
		mDepthStencilHandleCPU = DescriptorHeapManager::GetDsvCpuHandle(0);

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
	}

	void DX12Renderer::OnViewportResize(int width, int height)
	{
		if (md3dDevice == nullptr)
		{
			return;
		}
		assert(md3dDevice);
		assert(mSwapChain);
		assert(mDirectCmdListAlloc);
		// Flush before changing any resources.
		FlushCommandQueue();
		ThrowIfFailed(mCmdList->Reset(mDirectCmdListAlloc.Get(), nullptr));

		mViewPortBuffer->BuildResource(width, height, mBackBufferFormat);

		// Execute the resize commands.
		ThrowIfFailed(mCmdList->Close());
		ID3D12CommandList* cmdsLists[] = { mCmdList.Get() };
		mCmdQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		// Wait until resize is complete.
		FlushCommandQueue();
	}

	void DX12Renderer::CreateFrameResources()
	{
		for (int i = 0; i < mNumFrameResources; ++i)
		{
			// one pass cb for default, one for shadow
			mFrameResources.push_back(std::make_shared<FrameResource>(
				1, mObjectCBCount, mPassCBByteSize, mObjectCBByteSize, mSettingCBByteSize));
		}
	}

#pragma endregion InitDX12

#pragma region InputAssembly
	/*****************************************************************/
	/*********************** Input Assembly **************************/
	/*****************************************************************/
	uint32_t DX12Renderer::LoadTexture2D(std::string fileName)
	{
		std::wstring wFilePath(fileName.begin(), fileName.end());
		uint32_t id = TextureManager::LoadTexture(wFilePath, ETextureDimension::TEXTURE2D);

		return id;
	}

	uint32_t DX12Renderer::LoadTexture3D(std::string fileName)
	{
		std::wstring wFilePath(fileName.begin(), fileName.end());
		uint32_t id = TextureManager::LoadTexture(wFilePath, ETextureDimension::TEXTURE3D);

		return id;
	}

	uint32_t DX12Renderer::CreateRenderItem(uint32_t vertexCount, size_t vertexSize, BYTE* vertexData, std::vector<uint16_t> indices, uint32_t cbOffset)
	{
		auto ri = RenderItemManager::CreateRenderItem(RI_OPAQUE);
		ri->Init(vertexCount, vertexSize, vertexData, indices);
		ri->numFramesDirty = mNumFrameResources;
		ri->cbOffset = cbOffset;
		mObjectCBCount = max(mObjectCBCount, cbOffset + 1);
		return ri->renderItemID;
	}

	uint32_t DX12Renderer::CreateDebugRenderItem(uint32_t vertexCount, size_t vertexSize, BYTE* vertexData, std::vector<uint16_t> indices)
	{
		auto ri = RenderItemManager::CreateRenderItem(RI_OPAQUE);
		debugItem = ri;
		debugItem->Init(vertexCount, vertexSize, vertexData, indices);
		debugItem->debug = true;
		return debugItem->renderItemID;
	}
#pragma endregion InputAssembly

#pragma region InitPipeline
	

	/*****************************************************************/
	/*********************** Init Pipeline ***************************/
	/*****************************************************************/
	void DX12Renderer::InitPipeline()
	{
		LOG_TRACE("Init pipeline");

		// Build Root Sinagture
		RootSignatureManager::Init();

		// Init shader
		ShaderManager::Init();

		// build pso;
		PSOManager::Init(mBackBufferFormat, mDepthStencilFormat, m4xMsaaState, m4xMsaaQuality);
		PSOManager::BuildPSOs();

		ExecuteCommands();
		FlushCommandQueue();

		ShadowMap::Init(2048,2048);
	}

#pragma endregion InitPipeline

#pragma region Pipeline Update
	void DX12Renderer::UpdateFrame()
	{
		SwapFrameResource();
		UpdatePassCB();
		UpdateObjectCB();
		UpdateSettingCB();
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

	void DX12Renderer::UpdateSettingCB()
	{
		mCurrFrameResource->SettingCB->CopyData(0, mSettingCBData.data());
	}


	void DX12Renderer::UpdateRenderSettings(RenderSettings rs)
	{
		mRenderSetting = rs;
		BYTE data[sizeof(RenderSettings)];
		memcpy(&data, &rs, sizeof(rs));
		std::vector<BYTE> dataVector(data, data + sizeof(rs));
		mSettingCBData = dataVector;
	}

	void DX12Renderer::SetPassSceneData(BYTE* data)
	{
		mPassCBData = std::vector<BYTE>(data, data + mPassCBByteSize);
	}

	void DX12Renderer::SetObjectCB(uint32_t objCBIndex, BYTE* data)//XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale)
	{
		mObjectCBData[objCBIndex] = std::vector(data, data + mObjectCBByteSize);
		mObjectCBFramesDirty[objCBIndex] = mNumFrameResources;
	}

	void DX12Renderer::SetVisibility(uint32_t renderItemID, bool visible)
	{
		RenderItemManager::GetRenderItem(renderItemID)->visible = visible;
	}

	void DX12Renderer::SetTexture(uint32_t renderItemID, int texID, ETextureType type)
	{
		if (texID >= 0)
		{
			std::shared_ptr<RenderItem>& ri = RenderItemManager::GetRenderItem(renderItemID);
			std::shared_ptr<DX12Texture> tex = TextureManager::GetTexture(texID);
			switch(type)
			{
			case ETextureType::DIFFUSE:
				ri->diffuseHandle = TextureManager::GetTexture(texID)->handle;
				break;
			case ETextureType::SPECULAR:
				ri->specularHandle = TextureManager::GetTexture(texID)->handle;
				break;
			case ETextureType::NORMAL:
				ri->normalHandle = TextureManager::GetTexture(texID)->handle;
				break;
			case ETextureType::HEIGHT:
				ri->heightHandle = TextureManager::GetTexture(texID)->handle;
				break;
			default:
				break;
			}
		}
	}
#pragma endregion Pipeline Update

#pragma region Pipeline Draw
	void DX12Renderer::BindObjectCB(uint32_t id)
	{
		auto objectCBByteSize = UploadBufferUtil::CalcConstantBufferByteSize(mObjectCBByteSize);
		auto objectCB = mCurrFrameResource->ObjectCB->Resource();
		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + id * objectCBByteSize;
		mCmdList->SetGraphicsRootConstantBufferView(0, objCBAddress);
	}

	void DX12Renderer::BindAllMapToNull()
	{
		mCmdList->SetGraphicsRootDescriptorTable(3, TextureManager::NullTex3DHandle());
		mCmdList->SetGraphicsRootDescriptorTable(4, TextureManager::NullTex2DHandle());
		mCmdList->SetGraphicsRootDescriptorTable(5, TextureManager::NullTex2DHandle());
		mCmdList->SetGraphicsRootDescriptorTable(6, TextureManager::NullTex2DHandle());
		mCmdList->SetGraphicsRootDescriptorTable(7, TextureManager::NullTex2DHandle());
		mCmdList->SetGraphicsRootDescriptorTable(8, TextureManager::NullTex2DHandle());
	}

	void DX12Renderer::BindMap(uint32_t slot, D3D12_GPU_DESCRIPTOR_HANDLE handle)
	{
		mCmdList->SetGraphicsRootDescriptorTable(slot, handle);
	}

	void DX12Renderer::Render()
	{
		/* Stage 0: Reset */
		assert(mCmdList);
		auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;
		assert(cmdListAlloc);
		ThrowIfFailed(cmdListAlloc->Reset());
		ThrowIfFailed(mCmdList->Reset(cmdListAlloc.Get(), PSOManager::GetPSO("default").Get()));
		ID3D12DescriptorHeap* descriptorHeaps[] = { DescriptorHeapManager::GetSrvHeap().Get() };
		mCmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		mCmdList->SetGraphicsRootSignature(RootSignatureManager::GetRootSignature("default").Get());
		BindAllMapToNull();
		

		/* Stage 1: Generate Render-Based Resources*/
		if (mRenderSetting.sm_generateSM)
		{
			ShadowMap::BeginShadowMap(mPassCBByteSize, mCurrFrameResource->PassCB->Resource());
			RenderAllItems();
			ShadowMap::EndShadowMap();
		}

		/* Stage 2: Actual Render */
		RenderDefault();

		/* Stage 3: Post-Process */

		// Stage 4: Debug Plane */
		PSOManager::UsePSO("shadowDebug");
		RenderRenderItem(debugItem);
		
		/* Stage 5: End Render*/
		mViewPortBuffer->EndRender();
		// Set render target back to back buffer for ImGui
		mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		mCmdList->ClearRenderTargetView(CurrentBackBufferView(), DirectX::Colors::LightSteelBlue, 0, nullptr);
		mCmdList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
	}

	void DX12Renderer::RenderDefault()
	{
		mViewPortBuffer->StartRender(DepthStencilView());
		
		if (mRenderSetting.sm_generateSM)
		{
			PSOManager::UsePSO("default");
			BindMap(ETextureSlot::SLOT_SHADOW, ShadowMap::SrvGpuHandle());
		}
		else
		{
			PSOManager::UsePSO("default");
		}

		auto passCB = mCurrFrameResource->PassCB->Resource();
		auto settingCB = mCurrFrameResource->SettingCB->Resource();
		mCmdList->SetGraphicsRootConstantBufferView(1, passCB->GetGPUVirtualAddress());
		mCmdList->SetGraphicsRootConstantBufferView(2, settingCB->GetGPUVirtualAddress());
		RenderAllItems();

	}

	void DX12Renderer::RenderAllItems()
	{
		std::vector<std::shared_ptr<RenderItem>>& renderItems = RenderItemManager::GetAllRenderItems();
		for (auto& ri : renderItems)
		{
			if (ri->visible == false || ri->debug)
			{
				continue;
			}
			//LOG_INFO("ri id : {0},   ri diffuse: {1},   ri cb: {2}", ri->renderItemID, ri->diffuseOffset, ri->cbOffset);
			BindObjectCB(ri->cbOffset);
			BindMap(ETextureSlot::SLOT_DIFFUSE, ri->diffuseHandle);
			BindMap(ETextureSlot::SLOT_SPECULAR, ri->specularHandle);
			BindMap(ETextureSlot::SLOT_NORMAL, ri->normalHandle);
			BindMap(ETextureSlot::SLOT_HEIGHT, ri->heightHandle);
			mCmdList->IASetVertexBuffers(0, 1, &ri->vb.VertexBufferView());
			mCmdList->IASetIndexBuffer(&ri->ib.IndexBufferView());
			mCmdList->IASetPrimitiveTopology(ri->PrimitiveType);
			mCmdList->DrawIndexedInstanced(ri->indexCount, 1, 0, 0, 0);
		}
	}

	void DX12Renderer::RenderRenderItem(std::shared_ptr<RenderItem> ri)
	{
		if (ri->visible)
		{
			BindObjectCB(ri->cbOffset);
			BindMap(ETextureSlot::SLOT_SHADOW, ShadowMap::SrvGpuHandle());
			BindMap(ETextureSlot::SLOT_DIFFUSE, ri->diffuseHandle);
			BindMap(ETextureSlot::SLOT_SPECULAR, ri->specularHandle);
			BindMap(ETextureSlot::SLOT_NORMAL, ri->normalHandle);
			BindMap(ETextureSlot::SLOT_HEIGHT, ri->heightHandle);
			mCmdList->IASetVertexBuffers(0, 1, &ri->vb.VertexBufferView());
			mCmdList->IASetIndexBuffer(&ri->ib.IndexBufferView());
			mCmdList->IASetPrimitiveTopology(ri->PrimitiveType);
			mCmdList->DrawIndexedInstanced(ri->indexCount, 1, 0, 0, 0);
		}
	}

	void DX12Renderer::EndRender()
	{
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