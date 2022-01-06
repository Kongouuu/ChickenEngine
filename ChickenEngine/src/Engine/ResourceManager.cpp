#include "pch.h"
#include "Engine/ResourceManager.h"

namespace ChickenEngine
{
	uint32_t ResourceManager::LoadTexture(std::string path, std::string name)
	{
		Texture tex;
		tex.id = DX12Renderer::GetInstance().LoadTexture2D(path);
		LOG_ERROR("texid : {0}", tex.id);
		tex.path = path;
		instance().mTextures[tex.id] = tex;
		instance().name2id[name] = tex.id;
		return tex.id;
	}

	uint32_t ResourceManager::LoadTexture(std::string path)
	{
		Texture tex;
		tex.id = DX12Renderer::GetInstance().LoadTexture2D(path);
		LOG_ERROR("texid : {0}", tex.id);
		tex.path = path;
		instance().mTextures[tex.id] = tex;
		return tex.id;
	}
}

