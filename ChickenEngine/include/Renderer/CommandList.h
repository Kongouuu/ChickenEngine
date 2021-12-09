#pragma once

#include "Helper/DX12CommonHeader.h"

namespace ChickenEngine
{
	class CHICKEN_API CommandList : public Singleton<CommandList>
	{
	public:
		inline static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList() { return instance().mCmdList; }
		inline static void SetCmdList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _cmdList) { instance().mCmdList = _cmdList; }
	private:
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCmdList;
	};
}