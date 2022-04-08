#include "pch.h"
#include "Renderer/DX12Renderer/GBuffer.h"

namespace ChickenEngine
{
	void GBuffer::Init()
	{
		static bool bShaderPSOInit = false;
		if (bShaderPSOInit == false)
		{
			// init shader
			ShaderManager::LoadShader("gbuffer", "GBufferGenerate.hlsl", EShaderType::VERTEX_SHADER, "VS");
			ShaderManager::LoadShader("gbuffer", "GBufferGenerate.hlsl", EShaderType::PIXEL_SHADER, "PS");
			// init pso
			PSOManager::BuildPSO("gbuffer", "default", "gbuffer", "gbuffer", 3, DXGI_FORMAT_R16G16B16A16_FLOAT);
		}
		instance().mAlbedoBuffer.BuildResource(1, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);
		instance().mPositionBuffer.BuildResource(1, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);
		instance().mNormalBuffer.BuildResource(1, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);
		instance().mHandles[0] = instance().mAlbedoBuffer.GetRtvHandle();
		instance().mHandles[1] = instance().mPositionBuffer.GetRtvHandle();
		instance().mHandles[2] = instance().mNormalBuffer.GetRtvHandle();
	}

	void GBuffer::BuildResource(int width, int height)
	{
		instance().mWidth = width;
		instance().mHeight = height;

		instance().mViewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
		instance().mScissorRect = { 0, 0, (int)width, (int)height };

		instance().mAlbedoBuffer.BuildResource(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);
		instance().mPositionBuffer.BuildResource(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);
		instance().mNormalBuffer.BuildResource(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);
	}


	void GBuffer::BeginGBufferRender(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
	{
		PSOManager::UsePSO("gbuffer");
		CommandList::cmdList()->RSSetViewports(1, &instance().mViewport);
		CommandList::cmdList()->RSSetScissorRects(1, &instance().mScissorRect);
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(instance().mAlbedoBuffer.Resource().Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(instance().mPositionBuffer.Resource().Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(instance().mNormalBuffer.Resource().Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));

		CommandList::cmdList()->ClearRenderTargetView(instance().mHandles[0], DirectX::Colors::Black, 0, nullptr);
		CommandList::cmdList()->ClearRenderTargetView(instance().mHandles[1], DirectX::Colors::White, 0, nullptr);
		CommandList::cmdList()->ClearRenderTargetView(instance().mHandles[2], DirectX::Colors::Black, 0, nullptr);
		CommandList::cmdList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		CommandList::cmdList()->OMSetRenderTargets(3, instance().mHandles, true, &dsvHandle);
	}

	void GBuffer::EndGBufferRender()
	{
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(instance().mAlbedoBuffer.Resource().Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(instance().mPositionBuffer.Resource().Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(instance().mNormalBuffer.Resource().Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));

	}

}
