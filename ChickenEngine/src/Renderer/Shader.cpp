#include "pch.h"
#include "Renderer\Shader.h"
#include <tchar.h>


namespace ChickenEngine
{
	void ShaderManager::Init()
	{
		LOG_INFO("Shader - Init");
		instance().LoadVS();
		instance().LoadPS();
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


	void ShaderManager::LoadVS()
	{
		mVertexShaders["default"] = CompileShader(GetShaderPath("default.hlsl"), nullptr, "VS", "vs_5_1");
	}

	void ShaderManager::LoadPS()
	{
		mPixelShaders["default"] =  CompileShader(GetShaderPath("default.hlsl"), nullptr, "PS", "ps_5_1");
	}

	Microsoft::WRL::ComPtr<ID3DBlob> ShaderManager::CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
	{
		UINT compileFlags = 0;
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

	std::wstring ShaderManager::GetShaderPath(std::string name)
	{
		std::string _projectDir = STRINGIFY(PROJECT_DIR);
		_projectDir.erase(0, 1); // erase the first quote
		_projectDir.erase(_projectDir.size() - 2); // erase the last quote and the dot
		_projectDir += "shaders\\" + name;
		if (!std::filesystem::exists(_projectDir))
		{
			LOG_ERROR("File {0} does not exist", name);
			assert(0);
		}
		std::wstring projectDir = std::wstring(_projectDir.begin(), _projectDir.end());
		return projectDir;
	}

}
