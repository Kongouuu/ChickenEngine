#pragma once
#include "pch.h"
#include "Engine/Core.h"
#include "DX12CommonHeader.h"


namespace ChickenEngine
{
	class CHICKEN_API Shader
	{
	public:

		static void Init();
		static Microsoft::WRL::ComPtr<ID3DBlob> GetVS(std::string name);
		static Microsoft::WRL::ComPtr<ID3DBlob> GetPS(std::string name);

	private:
		Shader();
		~Shader();

		static Shader& GetInstance();
		void LoadVS();
		void LoadPS();
		Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
			const std::wstring& filename,
			const D3D_SHADER_MACRO* defines,
			const std::string& entrypoint,
			const std::string& target);

		std::wstring GetShaderPath(std::string name);

	private:
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mVertexShaders;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mPixelShaders;
	};

}

