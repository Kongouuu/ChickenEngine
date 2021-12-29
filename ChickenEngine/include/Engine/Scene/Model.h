#pragma once

#include "Mesh.h"
#include "Renderer/DX12Renderer.h"
#include "Engine/ResourceManager.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace ChickenEngine
{
	class CHICKEN_API Model
	{
	public:
		Model() {}
		Model(std::string path) { LoadModel(path); }
		std::vector<Mesh> mMeshes;
		std::vector<Texture> mTexturesLoaded;
		std::string directory;
		void LoadModel(std::string path);

		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
		Texture LoadMaterialTexture(aiMaterial* mat, aiTextureType type, ETextureType typeName);
	};
}


