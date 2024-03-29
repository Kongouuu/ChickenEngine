#include "pch.h"
#include "Renderer/DX12Renderer/PSO.h"

namespace ChickenEngine
{
	void PSOManager::Init(DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthStencilFormat, bool msaaState, uint32_t msaaQuality)
	{
		LOG_INFO("PSOManager - Init");
		instance().mBackBufferFormat = backBufferFormat;
		instance().mDepthStencilFormat = depthStencilFormat;
		instance().m4xMsaaState = msaaState;
		instance().m4xMsaaQuality = msaaQuality;
	}

	void PSOManager::BuildPSOs()
	{
		LOG_INFO("PSOManager - Build all PSO");
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = InputLayout::GetInputLayout();

		// default
		D3D12_GRAPHICS_PIPELINE_STATE_DESC defaultPsoDesc = DefaultPsoDesc();	
		defaultPsoDesc.InputLayout = { inputLayout.data(), (UINT)inputLayout.size() };
		defaultPsoDesc.pRootSignature = RootSignatureManager::GetRootSignature("default").Get();
		defaultPsoDesc.VS = VSByteCode("default");
		defaultPsoDesc.PS = PSByteCode("default");
		ThrowIfFailed(Device::device()->CreateGraphicsPipelineState(&defaultPsoDesc, IID_PPV_ARGS(&instance().mPSOs["default"])));

		// deferred
		D3D12_GRAPHICS_PIPELINE_STATE_DESC deferredPsoDesc = DefaultPsoDesc();
		deferredPsoDesc.pRootSignature = RootSignatureManager::GetRootSignature("default").Get();
		deferredPsoDesc.VS = VSByteCode("deferred");
		deferredPsoDesc.PS = PSByteCode("deferred");
		ThrowIfFailed(Device::device()->CreateGraphicsPipelineState(&deferredPsoDesc, IID_PPV_ARGS(&instance().mPSOs["deferred"])));

		// shadow map generation
		D3D12_GRAPHICS_PIPELINE_STATE_DESC smapPsoDesc = defaultPsoDesc;
		smapPsoDesc.RasterizerState.DepthBias = 50000;
		smapPsoDesc.RasterizerState.DepthBiasClamp = 0.0f;
		smapPsoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
		smapPsoDesc.VS = VSByteCode("shadow");
		smapPsoDesc.PS = PSByteCode("shadow");
		// Shadow map pass does not have a render target.
		smapPsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		smapPsoDesc.NumRenderTargets = 0;
		ThrowIfFailed(Device::device()->CreateGraphicsPipelineState(&smapPsoDesc, IID_PPV_ARGS(&instance().mPSOs["shadow"])));

		// vsm map generation
		smapPsoDesc.VS = VSByteCode("vsm");
		smapPsoDesc.PS = PSByteCode("vsm");
		smapPsoDesc.RTVFormats[0] = DXGI_FORMAT_R16G16_UNORM;
		smapPsoDesc.NumRenderTargets = 1;
		ThrowIfFailed(Device::device()->CreateGraphicsPipelineState(&smapPsoDesc, IID_PPV_ARGS(&instance().mPSOs["vsm"])));

		// debug
		D3D12_GRAPHICS_PIPELINE_STATE_DESC debugPsoDesc = defaultPsoDesc;
		debugPsoDesc.VS = VSByteCode("debug");
		debugPsoDesc.PS = PSByteCode("debug");
		debugPsoDesc.DepthStencilState.DepthEnable = false;
		ThrowIfFailed(Device::device()->CreateGraphicsPipelineState(&debugPsoDesc, IID_PPV_ARGS(&instance().mPSOs["debug"])));

		// shadow debug
		debugPsoDesc.PS = PSByteCode("shadowDebug");
		ThrowIfFailed(Device::device()->CreateGraphicsPipelineState(&debugPsoDesc, IID_PPV_ARGS(&instance().mPSOs["shadowDebug"])));

		// sky box generation
		D3D12_GRAPHICS_PIPELINE_STATE_DESC skyPsoDesc = defaultPsoDesc;
		skyPsoDesc.VS = VSByteCode("skyBox");
		skyPsoDesc.PS = PSByteCode("skyBox");
		skyPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		skyPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		ThrowIfFailed(Device::device()->CreateGraphicsPipelineState(&skyPsoDesc, IID_PPV_ARGS(&instance().mPSOs["skyBox"])));

	}

	Microsoft::WRL::ComPtr<ID3D12PipelineState>& PSOManager::GetPSO(std::string name)
	{
		if (instance().mPSOs.find(name) == instance().mPSOs.end())
		{
			LOG_ERROR("PSOManager - Get PSO fail");
			assert(0);
		}

		return instance().mPSOs[name];
	}

	void PSOManager::BuildPSO(std::string name, std::string rootSignature, std::string vsName, std::string psName)
	{
		// Can also use initializer; but for further development might have more inputs for the detailed function.
		// Some input might be kinda dynamic, cannot in initialized directly using inline
		BuildPSO(name, rootSignature, vsName, psName, 1, instance().mBackBufferFormat);
	}

	void PSOManager::BuildPSO(std::string name, std::string rootSignature, std::string vsName, std::string psName, int numRenderTargets, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat)
	{
		LOG_INFO("PSOManager - BuildPSO");
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = InputLayout::GetInputLayout();

		// default
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = DefaultPsoDesc();
		psoDesc.InputLayout = { inputLayout.data(), (UINT)inputLayout.size() };
		psoDesc.pRootSignature = RootSignatureManager::GetRootSignature(rootSignature).Get();
		psoDesc.NumRenderTargets = numRenderTargets;
		psoDesc.DSVFormat = dsvFormat;
		for(int i=0; i < numRenderTargets; i++)
			psoDesc.RTVFormats[i] = rtvFormat;
		psoDesc.VS = VSByteCode(vsName);
		psoDesc.PS = PSByteCode(psName);
		ThrowIfFailed(Device::device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&instance().mPSOs[name])));
	}

	void PSOManager::UsePSO(std::string name)
	{
		CommandList::cmdList()->SetPipelineState(GetPSO(name).Get());
	}
	
	const D3D12_GRAPHICS_PIPELINE_STATE_DESC& PSOManager::DefaultPsoDesc()
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC defaultPsoDesc = {};

		ZeroMemory(&defaultPsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

		defaultPsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		defaultPsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		defaultPsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		defaultPsoDesc.SampleMask = UINT_MAX;
		defaultPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		defaultPsoDesc.NumRenderTargets = 1;
		defaultPsoDesc.RTVFormats[0] = instance().mBackBufferFormat;
		defaultPsoDesc.SampleDesc.Count = instance().m4xMsaaState ? 4 : 1;
		defaultPsoDesc.SampleDesc.Quality = instance().m4xMsaaState ? (instance().m4xMsaaQuality - 1) : 0;
		defaultPsoDesc.DSVFormat = instance().mDepthStencilFormat;

		/*D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc;
		transparencyBlendDesc.BlendEnable = true;
		transparencyBlendDesc.LogicOpEnable = false;
		transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
		transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		defaultPsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;*/
		

		return defaultPsoDesc;
	}
}
