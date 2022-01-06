#include "pch.h"
#include "Renderer/DX12Renderer/FrameResource.h"

namespace ChickenEngine
{
    FrameResource::FrameResource(uint32_t passCount, uint32_t objectCount, uint32_t passCBSize, uint32_t objCBSize)
    {
        ThrowIfFailed(Device::device()->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));

        PassCB = std::make_unique<UploadBufferV2>(passCount, true, passCBSize);
        ObjectCB = std::make_unique<UploadBufferV2>(objectCount, true, objCBSize);

        //MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(materialCount, false);
        //ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(objectCount, true);
    }

}