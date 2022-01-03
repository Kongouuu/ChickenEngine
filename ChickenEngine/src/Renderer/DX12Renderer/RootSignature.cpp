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
		return instance().mRootSignatures[name];
	}

	void RootSignatureManager::LoadRootSignatures()
	{
		LOG_INFO("RootSignature - Load root signatures");
		//  ---------------- 简易root signature ----------------
		// Root parameter can be a table, root descriptor or root constants.
		CD3DX12_ROOT_PARAMETER slotRootParameter[8];


		// b0 为每个物体的变换矩阵
		slotRootParameter[0].InitAsConstantBufferView(0);
		// b2 为每一次pass的其他信息(例如摄像机，灯光)
		slotRootParameter[1].InitAsConstantBufferView(1);

		CD3DX12_DESCRIPTOR_RANGE skyTex;
		skyTex.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);

		CD3DX12_DESCRIPTOR_RANGE shadowTex;
		shadowTex.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0);

		CD3DX12_DESCRIPTOR_RANGE diffuseTex;
		diffuseTex.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0);

		CD3DX12_DESCRIPTOR_RANGE specularTex;
		specularTex.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3, 0);

		CD3DX12_DESCRIPTOR_RANGE normalTex;
		normalTex.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4, 0);

		CD3DX12_DESCRIPTOR_RANGE heightTex;
		heightTex.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5, 0);

		slotRootParameter[2].InitAsDescriptorTable(1, &skyTex, D3D12_SHADER_VISIBILITY_PIXEL);
		slotRootParameter[3].InitAsDescriptorTable(1, &shadowTex, D3D12_SHADER_VISIBILITY_PIXEL);
		slotRootParameter[4].InitAsDescriptorTable(1, &diffuseTex, D3D12_SHADER_VISIBILITY_PIXEL);
		slotRootParameter[5].InitAsDescriptorTable(1, &specularTex, D3D12_SHADER_VISIBILITY_PIXEL);
		slotRootParameter[6].InitAsDescriptorTable(1, &normalTex, D3D12_SHADER_VISIBILITY_PIXEL);
		slotRootParameter[7].InitAsDescriptorTable(1, &heightTex, D3D12_SHADER_VISIBILITY_PIXEL);

		auto staticSamplers = GetStaticSamplers();

		// A root signature is an array of root parameters.
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(8, slotRootParameter,
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

		mRootSignatures["default"] = nullptr;
		ThrowIfFailed(Device::device()->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(mRootSignatures["default"].GetAddressOf())));
		// other
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
