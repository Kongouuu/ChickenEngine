#include "pch.h"
#include "Engine/ResourceManager.h"

namespace ChickenEngine
{
	uint32_t ResourceManager::LoadTexture(std::string path, std::string name, ETextureDimension dimension)
	{
		uint32_t id = LoadTexture(path, dimension);
		instance().name2id[name] = id;
		return id;
	}

	uint32_t ResourceManager::LoadTexture(std::string path, ETextureDimension dimension)
	{
		Texture tex;
		if (dimension == ETextureDimension::TEXTURE2D)
			tex.id = DX12Renderer::GetInstance().LoadTexture2D(path);
		else
			tex.id = DX12Renderer::GetInstance().LoadTexture3D(path);
		LOG_ERROR("texid : {0}", tex.id);
		tex.path = path;
		instance().mTextures[tex.id] = tex;
		return tex.id;
	}
}

