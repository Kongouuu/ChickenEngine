#include "pch.h"
#include "Renderer\Shader.h"
#include <tchar.h>


namespace ChickenEngine
{
	Shader::Shader()
	{

	}

	Shader::~Shader()
	{
	}

	Shader& Shader::GetInstance()
	{
		static Shader instance;
		return instance;
	}

	void Shader::Init()
	{
		LOG_INFO("Shader - Init");
		GetInstance().LoadVS();
		GetInstance().LoadPS();
	}

	Microsoft::WRL::ComPtr<ID3DBlob> Shader::GetVS(std::string name)
	{
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>>& vsMap = GetInstance().mVertexShaders;
		if (vsMap.find(name) == vsMap.end())
			return nullptr;
		return vsMap[name];
	}

	Microsoft::WRL::ComPtr<ID3DBlob> Shader::GetPS(std::string name)
	{
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> &psMap = GetInstance().mPixelShaders;
		if (psMap.find(name) == psMap.end())
			return nullptr;
		return psMap[name];
	}


	void Shader::LoadVS()
	{
		mVertexShaders["default"] = CompileShader(GetShaderPath("default.hlsl"), nullptr, "VS", "vs_5_1");
	}

	void Shader::LoadPS()
	{
		mPixelShaders["default"] =  CompileShader(GetShaderPath("default.hlsl"), nullptr, "PS", "ps_5_1");
	}

	Microsoft::WRL::ComPtr<ID3DBlob> Shader::CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
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
		LOG_TRACE("before return");

		return byteCode;
	}

	std::wstring Shader::GetShaderPath(std::string name)
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
