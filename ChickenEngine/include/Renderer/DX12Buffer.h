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


	class CHICKEN_API DX12BufferManager
	{
	};

}

