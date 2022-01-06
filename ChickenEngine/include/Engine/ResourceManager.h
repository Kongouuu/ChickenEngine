#pragma once
#include <pch.h>
#include "Core.h"
#include "Helper/Singleton.h"
#include "Scene/RenderObject.h"
#include "Renderer/DX12Renderer/DX12Renderer.h"

namespace ChickenEngine
{

	class CHICKEN_API ResourceManager : public Singleton<ResourceManager>
	{
	public:
		inline static Texture& GetTexture(std::string name) { return GetTexture(instance().name2id[name]); }
		inline static Texture& GetTexture(uint32_t id) { return instance().mTextures[id]; }
		static uint32_t LoadTexture(std::string path, std::string name);
		static uint32_t LoadTexture(std::string path);
		
	private:
		std::unordered_map<UINT,Texture> mTextures;
		std::unordered_map<std::string, UINT> name2id;
	};


}
