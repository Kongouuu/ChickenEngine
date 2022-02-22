#pragma once
#include "pch.h"
#include "Helper/DX12CommonHeader.h"

namespace ChickenEngine
{
	enum EShaderType
	{
		VERTEX_SHADER = 0,
		PIXEL_SHADER,
		COMPUTE_SHADER
	};

	class CHICKEN_API ShaderManager : public Singleton<ShaderManager>
	{
	public:

		static void Init();
		static Microsoft::WRL::ComPtr<ID3DBlob> GetVS(std::string name);
		static Microsoft::WRL::ComPtr<ID3DBlob> GetPS(std::string name);
		static Microsoft::WRL::ComPtr<ID3DBlob> GetCS(std::string name);
		static void LoadShader(std::string name, std::string path, EShaderType st, std::string entry);

	private:

		void LoadDefaultVS();
		void LoadDefaultPS();
		Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
			const std::wstring& filename,
			const D3D_SHADER_MACRO* defines,
			const std::string& entrypoint,
			const std::string& target);

	private:
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mVertexShaders;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mPixelShaders;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mComputeShaders;
	};

}

