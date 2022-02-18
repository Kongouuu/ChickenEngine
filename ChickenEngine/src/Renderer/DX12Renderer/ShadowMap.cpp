#include "pch.h"
#include "Renderer/DX12Renderer/ShadowMap.h"

namespace ChickenEngine
{
	void ShadowMap::Init(uint32_t width, uint32_t height)
	{
		instance().mWidth = width;
		instance().mHeight = height;

		instance().mViewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
		instance().mScissorRect = { 0, 0, (int)width, (int)height };

		instance().BuildResource();
		instance().BuildDescriptors();
		instance().mSquaredShadowMap.BuildResource(width, height, DXGI_FORMAT_R16G16_UNORM, 8);
		instance().mSquaredShadowMap.EnableMipmap();
		LOG_INFO("");
	}

	void ShadowMap::OnResize(uint32_t width, uint32_t height)
	{
		if ((instance().mWidth != width) || (instance().mHeight != height))
		{
			instance().mWidth = width;
			instance().mHeight = height;
			instance().mViewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
			instance().mScissorRect = { 0, 0, (int)width, (int)height };

			instance().BuildResource();
			instance().BuildDescriptors();
		}
	}

	void ShadowMap::BuildResource()
	{
		D3D12_RESOURCE_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Alignment = 0;
		texDesc.Width = mWidth;
		texDesc.Height = mHeight;
		texDesc.DepthOrArraySize = 1;
		texDesc.MipLevels = 7;
		texDesc.Format = mFormat;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;

		ThrowIfFailed(Device::device()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			&optClear,
			IID_PPV_ARGS(&mShadowMap)));
	}

	void ShadowMap::BuildDescriptors()
	{
		/* Shadow Map*/
		if (mSrvOffset < 0)
		{
			mSrvOffset = DescriptorHeapManager::BindSrv(mShadowMap.Get(), ETextureDimension::TEXTURE2D, DXGI_FORMAT_R24_UNORM_X8_TYPELESS);
			mSrvGpuHandle = DescriptorHeapManager::GetSrvUavGpuHandle(mSrvOffset);
		}
		else
		{
			if (!DescriptorHeapManager::RebindSrv(mShadowMap.Get(), mSrvOffset))
				assert(0);
		}
		if (mDsvOffset < 0)
		{
			mDsvOffset = DescriptorHeapManager::BindDsv(mShadowMap.Get());
			mDsvCpuHandle = DescriptorHeapManager::GetDsvCpuHandle(mDsvOffset);
		}
		else
		{
			if (!DescriptorHeapManager::RebindDsv(mShadowMap.Get(), mDsvOffset))
				assert(0);
		}
	}

	void ShadowMap::BeginShadowMap(uint32_t passCBByteSize, Microsoft::WRL::ComPtr<ID3D12Resource> passCB)
	{
		auto& sm = instance();

		if (sm.bEnableVSM)
		{
			D3D12_FEATURE_DATA_D3D12_OPTIONS FeatureData;
			ZeroMemory(&FeatureData, sizeof(FeatureData));
			HRESULT hr = Device::device()->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &FeatureData, sizeof(FeatureData));
			if (SUCCEEDED(hr))
			{
				// TypedUAVLoadAdditionalFormats contains a Boolean that tells you whether the feature is supported or not
				if (FeatureData.TypedUAVLoadAdditionalFormats)
				{
					// Cannot assume other formats are supported, so we check:
					D3D12_FEATURE_DATA_FORMAT_SUPPORT FormatSupport = { DXGI_FORMAT_R16G16_UNORM, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE };
					hr = Device::device()->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &FormatSupport, sizeof(FormatSupport));
					if (SUCCEEDED(hr) && (FormatSupport.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
					{
					}
				}
			}
			else
			{
				assert(0);
			}
			PSOManager::UsePSO("vsm");
		}
		else
		{
			PSOManager::UsePSO("shadow");
		}
		
		auto& cmdList = CommandList::cmdList();

		cmdList->RSSetViewports(1, &sm.mViewport);
		cmdList->RSSetScissorRects(1, &sm.mScissorRect);
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(sm.mShadowMap.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));
		cmdList->ClearDepthStencilView(instance().mDsvCpuHandle,
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		if (sm.bEnableVSM)
		{
			sm.mSquaredShadowMap.StartRender(instance().mDsvCpuHandle, DirectX::Colors::White);
		}
		else
		{
			cmdList->OMSetRenderTargets(0, nullptr, false, &instance().mDsvCpuHandle);
		}
	

		D3D12_GPU_VIRTUAL_ADDRESS passCBAddress = passCB->GetGPUVirtualAddress();
		cmdList->SetGraphicsRootConstantBufferView(0, passCBAddress);
	}

	void ShadowMap::EndShadowMap()
	{
		if (instance().bEnableVSM)
		{
			instance().mSquaredShadowMap.EndRender();
		}
			
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(instance().mShadowMap.Get(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));
		
	}
	void ShadowMap::GenerateVSMMipMap()
	{
		instance().mSquaredShadowMap.GenerateMipmap();
	}
}