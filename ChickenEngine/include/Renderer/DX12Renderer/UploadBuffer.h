#pragma once

#include "Helper/DX12CommonHeader.h"
namespace ChickenEngine
{
	template<typename T>
	class CHICKEN_API UploadBuffer
	{
	public:
		UploadBuffer(uint32_t elementCount, bool isConstantBuffer) :mIsConstantBuffer(isConstantBuffer)
		{
			mElementByteSize = sizeof(T);
			if (isConstantBuffer)
				mElementByteSize = UploadBufferUtil::CalcConstantBufferByteSize(sizeof(T));

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


	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
		BYTE* mMappedData = nullptr;
		uint32_t mElementByteSize = 0;
		bool mIsConstantBuffer = false;
	};

	class CHICKEN_API UploadBufferUtil
	{
	public:
		inline static uint32_t CalcConstantBufferByteSize(uint32_t byteSize) { return (byteSize + 255) & ~255; }
	};




	/* Suppose use custom shader, will have to use this type of upload buffer*/
	/* So that renderer can support custom passCB */
	class CHICKEN_API UploadBufferV2
	{
	public:
		UploadBufferV2(uint32_t elementCount, bool isConstantBuffer, uint32_t byteSize) :mIsConstantBuffer(isConstantBuffer)
		{
			mElementByteSize = byteSize;
			mByteSize = byteSize;
			if (isConstantBuffer)
				mElementByteSize = CalcConstantBufferByteSize(byteSize);

			ThrowIfFailed(Device::device()->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize * elementCount),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&mUploadBuffer)));

			ThrowIfFailed(mUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData)));
		}

		UploadBufferV2(const UploadBufferV2& rhs) = delete;
		UploadBufferV2& operator=(const UploadBufferV2& rhs) = delete;
		~UploadBufferV2()
		{
			if (mUploadBuffer != nullptr)
				mUploadBuffer->Unmap(0, nullptr);

			mMappedData = nullptr;
		}

		inline ID3D12Resource* Resource()const { return mUploadBuffer.Get(); }

		inline void CopyData(int elementIndex, const BYTE* data) { memcpy(&mMappedData[elementIndex * mElementByteSize], data, mByteSize); }

		inline uint32_t CalcConstantBufferByteSize(uint32_t byteSize) { return (byteSize + 255) & ~255; }


	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
		BYTE* mMappedData = nullptr;
		uint32_t mElementByteSize = 0;
		uint32_t mByteSize = 0;
		bool mIsConstantBuffer = false;
	};
}