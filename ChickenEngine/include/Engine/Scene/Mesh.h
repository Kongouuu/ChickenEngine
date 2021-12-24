#pragma once


#include "Engine/Core.h"
#include "Engine/Log.h"
#include "Helper/Singleton.h"
#include "Helper/MathHelper.h"
#include "Helper/DX12Defines.h"
#include "pch.h"



namespace ChickenEngine
{
	struct Vertex
	{
		Vertex() {}

		Vertex(
			DirectX::XMFLOAT3 p,
			DirectX::XMFLOAT3 n,
			DirectX::XMFLOAT3 t,
			DirectX::XMFLOAT3 b,
			DirectX::XMFLOAT2 t1) :
			pos(p), normal(n),tangent(t), bitangent(b),texC(t1) {}

		
		DirectX::XMFLOAT3 pos = { 0.0,0.0,0.0 };
		DirectX::XMFLOAT3 normal = { 0.0,0.0,0.0 };
		DirectX::XMFLOAT3 tangent = { 0.0,0.0,0.0 };
		DirectX::XMFLOAT3 bitangent = { 0.0,0.0,0.0 };
		DirectX::XMFLOAT2 texC = { 0.0,0.0 };

	};

	enum ETextureType
	{
		DIFFUSE = 0,
		SPECULAR,
		NORMAL,
		HEIGHT,
		NULL_TYPE
	};

	struct Texture {
		UINT id = -1;
		ETextureType type = ETextureType::NULL_TYPE;
		std::string path;
		Texture() {}
		Texture(UINT id, ETextureType type, std::string path) : id(id), type(type), path(path) {}
	};

	struct Mesh
	{
		EVertexLayout layout;
		UINT renderItemID;
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
	};
}

