#pragma once
#include <pch.h>
#include "Core.h"

namespace ChickenEngine
{
	enum ETextureType
	{
		NORMAL = 0,
		CUBE
	};
	
	enum EDiffuseType
	{
		COLOR = 0,
		TEXTURE
	};

	struct RenderObject
	{
		std::string name;
		uint objID;
		EDiffuseType diffuseType;

		float position[3] = { 0.0,0.0,0.0 };
		float rotation[3] = { 0.0,0.0,0.0 };
		float scale[3] = { 1.0, 1.0, 1.0 };
		float color[3] = { 1.0, 0.0, 0.0 };
		std::vector<uint> meshIDs;
		std::vector<uint> texIDs;

		RenderObject(std::string _name, EDiffuseType _diffuseType) : name(_name), diffuseType(_diffuseType){}
	};

	struct Texture
	{
		std::string name;
		uint texID;
	};

	class ResourceManager
	{
	public:
		static ResourceManager& GetInstance();
		static RenderObject& GetRenderObject(std::string name);
		static RenderObject& GetRenderObject(uint ID);
		static Texture& GetTexture(std::string name);
		static Texture& GetTexture(uint ID);

		static RenderObject& LoadObject(std::string name);
		
	private:
		std::vector<RenderObject> mObjects;
		std::vector<Texture> mTextures;

	};


}
