#pragma once
#include <pch.h>
#include "Core.h"
#include "Helper/Singleton.h"
#include "Scene/RenderObject.h"

namespace ChickenEngine
{
	struct Texture
	{
		std::string name;
		UINT texID;
	};

	class CHICKEN_API ResourceManager : public Singleton<ResourceManager>
	{
	public:
		static RenderObject& GetRenderObject(std::string name);
		static RenderObject& GetRenderObject(UINT ID);
		static Texture& GetTexture(std::string name);
		static Texture& GetTexture(UINT ID);
		
	private:
		std::vector<RenderObject> mObjects;
		std::vector<Texture> mTextures;

	};


}
