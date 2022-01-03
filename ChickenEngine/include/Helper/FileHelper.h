#pragma once
#include "pch.h"
#include "Engine/Core.h"
#include "Engine/Log.h"

#define CONCAT2(X, Y) X##Y
#define CONCAT(X, Y) CONCAT2(X, Y)
#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)
#define WIDEN(X) CONCAT(L, STRINGIFY(X))


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

