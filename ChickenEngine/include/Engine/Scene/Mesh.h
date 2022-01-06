#pragma once

#include "Interface/IResource.h"
#include "Engine/Core.h"
#include "Engine/Log.h"
#include "Helper/Singleton.h"
#include "Helper/MathHelper.h"
#include "pch.h"

namespace ChickenEngine
{
	struct Texture {
		int id = -1;
		ETextureType type = ETextureType::NULL_TYPE;
		std::string path;
		Texture() {}
		Texture(uint32_t id, ETextureType type, std::string path) : id(id), type(type), path(path) {}
	};

	struct CHICKEN_API Mesh
	{
		uint32_t renderItemID;
		bool debug = false;
		std::vector<Vertex> vertices;
		std::vector<UINT> indices;
		Texture diffuseMap;
		Texture specularMap;
		Texture normalMap;
		Texture heightMap;
		std::vector<uint16_t>& GetIndices16()
		{
			if (indices16.empty())
			{
				indices16.resize(indices.size());
				for (size_t i = 0; i < indices.size(); ++i)
				{
					indices16[i] = static_cast<uint16_t>(indices[i]);
				}
			}
			return indices16;
		}

		void AddDiffuseTexture(Texture tex);
	private:
		std::vector<uint16_t> indices16;
	};

	class CHICKEN_API MeshManager : public Singleton<MeshManager>
	{
	public:
		static Mesh GenerateBox();
		static Mesh GeneratePlane();
		static Mesh GenerateDebugPlane();
	};
}

