#pragma once

#include <DirectXMath.h>
#include "Helper/MathHelper.h"
#include "Helper/DX12CommonHeader.h"
#include "UploadBuffer.h"
#include "Device.h"

namespace ChickenEngine
{

	class FrameResource
	{
	public:
		FrameResource(UINT passCount, UINT objectCount, UINT materialCount, UINT passCBSize, UINT objCBSize, UINT matCBSize);
		FrameResource(const FrameResource& rhs) = delete;
		FrameResource& operator=(const FrameResource& rhs) = delete;
		~FrameResource() {}

		// We cannot reset the allocator until the GPU is done processing the commands.
		// So each frame needs their own allocator.
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

		// We cannot update a cbuffer until the GPU is done processing the commands
		// that reference it.  So each frame needs their own cbuffers.

		std::unique_ptr<UploadBufferV2> PassCB = nullptr;
		std::unique_ptr<UploadBufferV2> ObjectCB = nullptr;
		std::unique_ptr<UploadBufferV2> MaterialCB = nullptr;

		//std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;
		//std::unique_ptr<UploadBuffer<MaterialConstants>> MaterialCB = nullptr;

		// Fence value to mark commands up to this fence point.  This lets us
		// check if these frame resources are still in use by the GPU.
		UINT64 Fence = 0;
	};


}

