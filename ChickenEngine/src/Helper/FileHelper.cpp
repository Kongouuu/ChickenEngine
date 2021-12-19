#include "pch.h"
#include "Helper\FileHelper.h"
#
namespace ChickenEngine
{
	std::wstring FileHelper::String2WString(std::string str)
	{
		std::wstring wstr(str.begin(), str.end());
		return wstr;
	}

	std::wstring FileHelper::GetShaderPath(std::string name)
	{
		std::string filePath = GetProjectPath();
		filePath += "shaders\\" + name;
		if (!std::filesystem::exists(filePath))
		{
			LOG_ERROR("File {0} does not exist", name);
			assert(0);
		}
		std::wstring wFilePath = String2WString(filePath);
		return wFilePath;
	}

	std::wstring FileHelper::GetTexturePath(std::string name)
	{
		std::string filePath = GetProjectPath();
		filePath += "assets\\textures\\" + name;
		
		if (!std::filesystem::exists(filePath))
		{
			LOG_ERROR("File {0} does not exist", filePath);
			assert(0);
		}
		std::wstring wFilePath = String2WString(filePath);
		return wFilePath;
	}

	std::string FileHelper::GetProjectPath()
	{
		std::string projectDir = STRINGIFY(PROJECT_DIR);
		projectDir.erase(0, 1); // erase the first quote
		projectDir.erase(projectDir.size() - 2); // erase the last quote and the dot
		LOG_TRACE("proj path : {0}", projectDir.c_str());
		return projectDir;
	}
}

