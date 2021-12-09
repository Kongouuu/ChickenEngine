#pragma once

#include "Helper/DX12CommonHeader.h"
namespace ChickenEngine
{
	template<typename T>
	class CHICKEN_API UploadBuffer
	{
	public:
		UploadBuffer(UINT elementCount, bool isConstantBuffer) :mIsConstantBuffer(isConstantBuffer)
		{
			mElementByteSize = sizeof(T);

			if (isConstantBuffer)
				mElementByteSize = CalcConstantBufferByteSize(sizeof(T));

			ThrowIfFailed(Device::device()->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize * elementCount),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&mUploadBuffer)));

			ThrowIfFailed(mUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData)));
		}

		UploadBuffer(const UploadBuffer& rhs) = delete;
		UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
		~UploadBuffer()
		{
			if (mUploadBuffer != nullptr)
				mUploadBuffer->Unmap(0, nullptr);

			mMappedData = nullptr;
		}

		inline ID3D12Resource* Resource()const { return mUploadBuffer.Get(); }

		inline void CopyData(int elementIndex, const T& data) { memcpy(&mMappedData[elementIndex * mElementByteSize], &data, sizeof(T)); }

		inline UINT CalcConstantBufferByteSize(UINT byteSize) { return (byteSize + 255) & ~255; }


	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
		BYTE* mMappedData = nullptr;
		UINT mElementByteSize = 0;
		bool mIsConstantBuffer = false;
	};
}