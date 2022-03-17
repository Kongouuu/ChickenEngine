#include "pch.h"
#include "Renderer/DX12Renderer/Shader.h"
#include <tchar.h>


namespace ChickenEngine
{
	using namespace Microsoft;
	void ShaderManager::Init()
	{
		LOG_INFO("Shader - Init");
		instance().LoadDefaultVS();
		instance().LoadDefaultPS();
	}

	Microsoft::WRL::ComPtr<ID3DBlob> ShaderManager::GetVS(std::string name)
	{
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>>& vsMap = instance().mVertexShaders;
		if (vsMap.find(name) == vsMap.end())
		{
			LOG_ERROR("ShaderManager - No VertexShader Found");
			assert(0);
			return nullptr;
		}
			
		return vsMap[name];
	}

	Microsoft::WRL::ComPtr<ID3DBlob> ShaderManager::GetPS(std::string name)
	{
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> &psMap = instance().mPixelShaders;
		if (psMap.find(name) == psMap.end())
		{
			LOG_ERROR("ShaderManager - No PixelShader Found");
			assert(0);
			return nullptr;
		};
		return psMap[name];
	}

	Microsoft::WRL::ComPtr<ID3DBlob> ShaderManager::GetCS(std::string name)
	{
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>>& csMap = instance().mComputeShaders;
		if (csMap.find(name) == csMap.end())
		{
			LOG_ERROR("ShaderManager - No ComputeShader Found");
			assert(0);
			return nullptr;
		};
		return csMap[name];
	}

	void ShaderManager::LoadShader(std::string name, std::string path, EShaderType st, std::string entry)
	{
		switch (st)
		{
		case EShaderType::VERTEX_SHADER:
			instance().mVertexShaders[name] = instance().CompileShader(FileHelper::String2WString(FileHelper::GetShaderPath(path)), nullptr, entry, "vs_5_1");
			break;
		case EShaderType::PIXEL_SHADER:
			instance().mPixelShaders[name] = instance().CompileShader(FileHelper::String2WString(FileHelper::GetShaderPath(path)), nullptr, entry, "ps_5_1");
			break;
		case EShaderType::COMPUTE_SHADER:
			instance().mComputeShaders[name] = instance().CompileShader(FileHelper::String2WString(FileHelper::GetShaderPath(path)), nullptr, entry, "cs_5_0");
			break;
		default:
			break;
		}
	}

	void ShaderManager::LoadDefaultVS()
	{
		// normal
		mVertexShaders["default"] = CompileShader(FileHelper::String2WString(FileHelper::GetShaderPath("Default.hlsl")), nullptr, "VS", "vs_5_1");
		// shadow
		mVertexShaders["shadow"] = CompileShader(FileHelper::String2WString(FileHelper::GetShaderPath("ShadowGenerate.hlsl")), nullptr, "VS", "vs_5_1");
		mVertexShaders["vsm"] = CompileShader(FileHelper::String2WString(FileHelper::GetShaderPath("VSMGenerate.hlsl")), nullptr, "VS", "vs_5_1");
		// shadow debug
		mVertexShaders["shadowDebug"] = CompileShader(FileHelper::String2WString(FileHelper::GetShaderPath("ShadowDebug.hlsl")), nullptr, "VS", "vs_5_1");
		// sky box
		mVertexShaders["skyBox"] = CompileShader(FileHelper::String2WString(FileHelper::GetShaderPath("SkyBox.hlsl")), nullptr, "VS", "vs_5_1");
	}

	void ShaderManager::LoadDefaultPS()
	{
		const D3D_SHADER_MACRO smDefines[] =
		{
			"SHADOW_MAP_ENABLED", "1",
			NULL, NULL
		};
		// normal
		mPixelShaders["default"] =  CompileShader(FileHelper::String2WString(FileHelper::GetShaderPath("Default.hlsl")), nullptr, "PS", "ps_5_1");
		// shadow
		mPixelShaders["shadow"] = CompileShader(FileHelper::String2WString(FileHelper::GetShaderPath("ShadowGenerate.hlsl")), nullptr, "PS", "ps_5_1");
		mPixelShaders["vsm"] = CompileShader(FileHelper::String2WString(FileHelper::GetShaderPath("VSMGenerate.hlsl")), nullptr, "PS", "ps_5_1");

		// shadow debug
		mPixelShaders["shadowDebug"] = CompileShader(FileHelper::String2WString(FileHelper::GetShaderPath("ShadowDebug.hlsl")), nullptr, "PS", "ps_5_1");
		// sky box
		mPixelShaders["skyBox"] = CompileShader(FileHelper::String2WString(FileHelper::GetShaderPath("SkyBox.hlsl")), nullptr, "PS", "ps_5_1");
	}

	Microsoft::WRL::ComPtr<ID3DBlob> ShaderManager::CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
	{
		uint32_t compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		HRESULT hr = S_OK;

		Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errors;
		hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

		if (errors != nullptr)
		{
			LOG_ERROR((char*)errors->GetBufferPointer());
		}
			

		if (FAILED(hr))
		{
			assert(0);
		}

		return byteCode;
	}

}
