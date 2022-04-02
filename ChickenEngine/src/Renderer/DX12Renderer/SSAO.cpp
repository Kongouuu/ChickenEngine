#include "pch.h"
#include "Renderer/DX12Renderer/SSAO.h"

namespace ChickenEngine
{
	void SSAO::Init(int width, int height)
	{
		static bool bShaderPSOInit = false;
		if (bShaderPSOInit == false)
		{
			// init shader
			ShaderManager::LoadShader("ssao", "SSAOGenerate.hlsl", EShaderType::VERTEX_SHADER, "VS");
			ShaderManager::LoadShader("ssao", "SSAOGenerate.hlsl", EShaderType::PIXEL_SHADER, "PS");
			// init pso
			PSOManager::BuildPSO("ssao", "default", "ssao", "ssao", 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN);
		}

		instance().mWidth = width;
		instance().mHeight = height;

		instance().mViewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
		instance().mScissorRect = { 0, 0, (int)width, (int)height };

		instance().mSSAOBuffer.BuildResource(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);

		instance().BuildOffsetVectors();
		instance().BuildRandomVectorTexture();
	}

	void SSAO::OnResize(int width, int height)
	{
		instance().mWidth = width;
		instance().mHeight = height;

		instance().mViewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
		instance().mScissorRect = { 0, 0, (int)width, (int)height };

		instance().mSSAOBuffer.BuildResource(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);
	}

	void SSAO::SSAORender()
	{
		auto& cmdList = CommandList::cmdList();
		cmdList->RSSetViewports(1, &instance().mViewport);
		cmdList->RSSetScissorRects(1, &instance().mScissorRect);

		
		// We compute the initial SSAO to AmbientMap0.

		// Change to RENDER_TARGET.
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(instance().mSSAOBuffer.Resource().Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));

		float clearValue[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		cmdList->ClearRenderTargetView(instance().mSSAOBuffer.GetRtvHandle(), clearValue, 0, nullptr);

		// Specify the buffers we are going to render to.
		cmdList->OMSetRenderTargets(1, &instance().mSSAOBuffer.GetRtvHandle(), true, nullptr);

		cmdList->SetGraphicsRootDescriptorTable(6, instance().mRandomVectorHandle);
		// Draw fullscreen quad.
		cmdList->IASetVertexBuffers(0, 0, nullptr);
		cmdList->IASetIndexBuffer(nullptr);
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->DrawInstanced(6, 1, 0, 0);

		// Change back to GENERIC_READ so we can read the texture in a shader.
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(instance().mSSAOBuffer.Resource().Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
	}

	// DX12 book
	void SSAO::BuildRandomVectorTexture()
	{
		D3D12_RESOURCE_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Alignment = 0;
		texDesc.Width = 256;
		texDesc.Height = 256;
		texDesc.DepthOrArraySize = 1;
		texDesc.MipLevels = 1;
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ThrowIfFailed(Device::device()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&mRandomVectorMap)));

		//
		// In order to copy CPU memory data into our default buffer, we need to create
		// an intermediate upload heap. 
		//

		const UINT num2DSubresources = texDesc.DepthOrArraySize * texDesc.MipLevels;
		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(mRandomVectorMap.Get(), 0, num2DSubresources);

		ThrowIfFailed(Device::device()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(mRandomVectorMapUploadBuffer.GetAddressOf())));

		DirectX::PackedVector::XMCOLOR initData[256 * 256];
		for (int i = 0; i < 256; ++i)
		{
			for (int j = 0; j < 256; ++j)
			{
				// Random vector in [0,1].  We will decompress in shader to [-1,1].
				XMFLOAT3 v(MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF());

				initData[i * 256 + j] = DirectX::PackedVector::XMCOLOR(v.x, v.y, v.z, 0.0f);
			}
		}

		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = initData;
		subResourceData.RowPitch = 256 * sizeof(DirectX::PackedVector::XMCOLOR);
		subResourceData.SlicePitch = subResourceData.RowPitch * 256;

		//
		// Schedule to copy the data to the default resource, and change states.
		// Note that mCurrSol is put in the GENERIC_READ state so it can be 
		// read by a shader.
		//

		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRandomVectorMap.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));
		UpdateSubresources(CommandList::cmdList().Get(), mRandomVectorMap.Get(), mRandomVectorMapUploadBuffer.Get(),
			0, 0, num2DSubresources, &subResourceData);
		CommandList::cmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRandomVectorMap.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

		int offset = DescriptorHeapManager::BindSrv(mRandomVectorMap.Get());
		mRandomVectorHandle = DescriptorHeapManager::GetSrvUavGpuHandle(offset);
	}

	void SSAO::BuildOffsetVectors()
	{
		mOffsets[0] = XMFLOAT4(+1.0f, +1.0f, +1.0f, 0.0f);
		mOffsets[1] = XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.0f);

		mOffsets[2] = XMFLOAT4(-1.0f, +1.0f, +1.0f, 0.0f);
		mOffsets[3] = XMFLOAT4(+1.0f, -1.0f, -1.0f, 0.0f);

		mOffsets[4] = XMFLOAT4(+1.0f, +1.0f, -1.0f, 0.0f);
		mOffsets[5] = XMFLOAT4(-1.0f, -1.0f, +1.0f, 0.0f);

		mOffsets[6] = XMFLOAT4(-1.0f, +1.0f, -1.0f, 0.0f);
		mOffsets[7] = XMFLOAT4(+1.0f, -1.0f, +1.0f, 0.0f);

		// 6 centers of cube faces
		mOffsets[8] = XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
		mOffsets[9] = XMFLOAT4(+1.0f, 0.0f, 0.0f, 0.0f);

		mOffsets[10] = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
		mOffsets[11] = XMFLOAT4(0.0f, +1.0f, 0.0f, 0.0f);

		mOffsets[12] = XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
		mOffsets[13] = XMFLOAT4(0.0f, 0.0f, +1.0f, 0.0f);

		for (int i = 0; i < 14; ++i)
		{
			// Create random lengths in [0.25, 1.0].
			float s = MathHelper::RandF(0.25f, 1.0f);

			XMVECTOR v = s * XMVector4Normalize(XMLoadFloat4(&mOffsets[i]));

			XMStoreFloat4(&mOffsets[i], v);
		}
	}
}

