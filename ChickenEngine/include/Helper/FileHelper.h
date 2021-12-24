#pragma once
#include "pch.h"
#include "Engine/Core.h"
#include "DX12Defines.h"
#include "Engine/Log.h"

namespace ChickenEngine
{
	class CHICKEN_API FileHelper
	{
	public:
		static std::wstring String2WString(std::string str);

		static std::string GetProjectPath();

		static std::string GetShaderPath(std::string name);
		static std::string GetTexturePath(std::string name);
		static std::string GetModelPath(std::string name);
		
	};
}

