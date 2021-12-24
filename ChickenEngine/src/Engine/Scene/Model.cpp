#include "pch.h"
#include "Engine\Scene\Model.h"

namespace ChickenEngine
{
	void Model::LoadModel(std::string path)
	{
		// read file via ASSIMP
		Assimp::Importer importer; 
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
			return;
		}
		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		// process ASSIMP's root node recursively
		ProcessNode(scene->mRootNode, scene);
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene)
	{
		for (UINT i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			mMeshes.push_back(ProcessMesh(mesh, scene));
		}

		for (UINT i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		// data to fill
		std::vector<Vertex> vertices;
		std::vector<UINT> indices;

		// walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

			vertex.pos.x = mesh->mVertices[i].x;
			vertex.pos.y = mesh->mVertices[i].y;
			vertex.pos.z = mesh->mVertices[i].z;

			// normals
			if (mesh->HasNormals())
			{
				vertex.normal.x = mesh->mNormals[i].x;
				vertex.normal.y = mesh->mNormals[i].y;
				vertex.normal.z = mesh->mNormals[i].z;
			}
			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				vertex.texC.x = mesh->mTextureCoords[0][i].x;
				vertex.texC.y = mesh->mTextureCoords[0][i].y;
				// tangent
				vertex.tangent.x = mesh->mTangents[i].x;
				vertex.tangent.y = mesh->mTangents[i].y;
				vertex.tangent.z = mesh->mTangents[i].z;
				// bitangent
				vertex.bitangent.x = mesh->mBitangents[i].x;
				vertex.bitangent.y = mesh->mBitangents[i].y;
				vertex.bitangent.z = mesh->mBitangents[i].z;
			}

			vertices.push_back(vertex);
		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// process materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		Mesh m;
		// 1. diffuse map
		m.diffuseMap = LoadMaterialTexture(material, aiTextureType_DIFFUSE, ETextureType::DIFFUSE);
		// 2. specular map
		m.specularMap = LoadMaterialTexture(material, aiTextureType_SPECULAR, ETextureType::SPECULAR);
		// 3. normal map
		m.normalMap = LoadMaterialTexture(material, aiTextureType_NORMALS, ETextureType::NORMAL);
		// 4. height map
		m.heightMap = LoadMaterialTexture(material, aiTextureType_HEIGHT, ETextureType::HEIGHT);

		m.vertices = vertices;
		m.indices = indices;

		return m;
	}

	Texture Model::LoadMaterialTexture(aiMaterial* mat, aiTextureType type, ETextureType typeName)
	{
		Texture texture;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (unsigned int j = 0; j < mTexturesLoaded.size(); j++)
			{
				if (std::strcmp(mTexturesLoaded[j].path.data(), str.C_Str()) == 0)
				{
					texture = mTexturesLoaded[j];
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{   // if texture hasn't been loaded already, load it
				texture.id = ResourceManager::LoadTexture(this->directory + "/" + str.C_Str());
				texture.type = typeName;
				texture.path = str.C_Str();

				mTexturesLoaded.push_back(texture);
			}
		}
		return texture;
	}
}

