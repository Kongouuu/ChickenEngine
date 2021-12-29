#include "pch.h"
#include "Renderer/FrameResource.h"

namespace ChickenEngine
{
    FrameResource::FrameResource(UINT passCount, UINT objectCount, UINT passCBSize, UINT objCBSize)
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