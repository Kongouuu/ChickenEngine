#pragma once

#include "Helper/DX12CommonHeader.h"

namespace ChickenEngine
{
	class CHICKEN_API Device : public Singleton<Device>
	{
	public:
		inline static Microsoft::WRL::ComPtr<ID3D12Device>& device() { return instance().md3dDevice; }
		inline static void SetDevice(Microsoft::WRL::ComPtr<ID3D12Device> _device) { instance().md3dDevice = _device; }
	private:
		Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
	};
}