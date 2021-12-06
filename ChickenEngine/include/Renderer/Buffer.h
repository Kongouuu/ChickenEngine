#pragma once
#include "DX12CommonHeader.h"
#include "Engine/Core.h"

namespace ChickenEngine
{
	enum EBufferType
	{
		VERTEX_BUFFER = 0,
		INDEX_BUFFER,
	};
	/*enum EVertexLayout
	{
		POS_NORM_COL = 0,
		POS_NORM_TEX,
		POS_NORM_TAN_TEX,
		NONE
	};*/


	struct VertexBuffer
	{
		Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;

		// Data about the buffers.
		UINT VertexByteStride = 0;
		UINT VertexBufferByteSize = 0;

		D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
		{
			D3D12_VERTEX_BUFFER_VIEW vbv;
			vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
			vbv.StrideInBytes = VertexByteStride;
			vbv.SizeInBytes = VertexBufferByteSize;

			return vbv;
		}
	};

	struct IndexBuffer
	{
		Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

		DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
		UINT IndexBufferByteSize = 0;


		D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
		{
			D3D12_INDEX_BUFFER_VIEW ibv;
			ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
			ibv.Format = IndexFormat;
			ibv.SizeInBytes = IndexBufferByteSize;

			return ibv;
		}
	};


	class CHICKEN_API BufferManager
	{
	public:
		static BufferManager& GetInstance();
		static void InitBufferManager(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);
		static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
			const void* initData,
			UINT64 byteSize,
			Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);
	private:
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
	};

}

