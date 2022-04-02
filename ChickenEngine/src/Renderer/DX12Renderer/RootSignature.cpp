#include "pch.h"
#include "Renderer/DX12Renderer/RootSignature.h"

namespace ChickenEngine
{

	void RootSignatureManager::Init()
	{
		LOG_INFO("RootSignature - Init");
		instance().LoadRootSignatures();
	}

	Microsoft::WRL::ComPtr<ID3D12RootSignature>& RootSignatureManager::GetRootSignature(std::string name)
	{
		if (instance().mRootSignatures.find(name) == instance().mRootSignatures.end())
			LOG_ERROR("no such root signature found");
		return instance().mRootSignatures[name];
	}

	void RootSignatureManager::LoadRootSignatures()
	{
		LOG_INFO("RootSignature - Load root signatures");
		//  ---------------- ¼òÒ×root signature ----------------
		// 3cbv: objectcb, passcb, setting cb
		// 6srv: sky, shadow, diffuse, specular, normal, height, ssao
		CreateRootSignatureSimple("default", 3, 7, 0);
	}

	void RootSignatureManager::CreateRootSignatureSimple(std::string name, int numCBV, int numSRV, int numUAV)
	{
		int totalNum = numCBV + numSRV + numUAV;
		std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter = std::vector<CD3DX12_ROOT_PARAMETER>(totalNum);
		for (int i = 0; i < numCBV; i++)
		{
			slotRootParameter[i].InitAsConstantBufferView(i);
		}

		std::vector<CD3DX12_DESCRIPTOR_RANGE> tex = std::vector < CD3DX12_DESCRIPTOR_RANGE>(numSRV);
		for (int i = 0; i < numSRV; i++)
		{
			tex[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, i, 0);
		}
		for (int i = 0; i < numSRV; i++)
		{
			slotRootParameter[i + numCBV].InitAsDescriptorTable(1, &tex[i], D3D12_SHADER_VISIBILITY_PIXEL);
		}

		std::vector<CD3DX12_DESCRIPTOR_RANGE> uavtex = std::vector < CD3DX12_DESCRIPTOR_RANGE>(numUAV);
		for (int i = 0; i < numUAV; i++)
		{
			uavtex[i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, i, 0);
		}
		for (int i = 0; i < numUAV; i++)
		{
			slotRootParameter[i + numCBV + numSRV].InitAsDescriptorTable(1, &uavtex[i]);
		}

		auto staticSamplers = GetStaticSamplers();

		// A root signature is an array of root parameters.
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(totalNum, slotRootParameter.data(),
			(UINT)staticSamplers.size(), staticSamplers.data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
		Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

		if (errorBlob != nullptr)
		{
			LOG_ERROR((char*)errorBlob->GetBufferPointer());
		}

		mRootSignatures[name] = nullptr;
		ThrowIfFailed(Device::device()->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(mRootSignatures[name].GetAddressOf())));
	}

	void RootSignatureManager::CreateRootSignatureFromParam(std::string name, std::vector<CD3DX12_ROOT_PARAMETER> param, int numSlot)
	{
		auto staticSamplers = GetStaticSamplers();

		// A root signature is an array of root parameters.
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(numSlot, param.data(),
			(UINT)staticSamplers.size(), staticSamplers.data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
		Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

		if (errorBlob != nullptr)
		{
			LOG_ERROR((char*)errorBlob->GetBufferPointer());
		}

		mRootSignatures[name] = nullptr;
		ThrowIfFailed(Device::device()->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(mRootSignatures[name].GetAddressOf())));
	}

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> RootSignatureManager::GetStaticSamplers()
	{
		// Applications usually only need a handful of samplers.  So just define them all up front
		// and keep them available as part of the root signature.  

		const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
			0, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
			1, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
			2, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
			3, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
			4, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
			0.0f,                             // mipLODBias
			8);                               // maxAnisotropy

		const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
			5, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
			0.0f,                              // mipLODBias
			8);                                // maxAnisotropy

		const CD3DX12_STATIC_SAMPLER_DESC shadow(
			6, // shaderRegister
			D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
			0.0f,                               // mipLODBias
			16,                                 // maxAnisotropy
			D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

		return {
			pointWrap, pointClamp,
			linearWrap, linearClamp,
			anisotropicWrap, anisotropicClamp,
			shadow
		};
	}
}
