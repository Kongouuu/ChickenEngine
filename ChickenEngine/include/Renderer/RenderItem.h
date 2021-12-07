#pragma once

#include "DX12CommonHeader.h"
#include "Buffer.h"
#include "MathHelper.h"
#include "Mesh.h"

namespace ChickenEngine
{
	enum ERenderItemType
	{
		RI_OPAQUE = 0,
		RI_TRANSPARENT,
		RI_OTHER
	};

	struct Material
	{
		Material() {}
		Material(float _roughness, float _metallic) : roughness(_roughness), metallic(_metallic){}

		float roughness = 0.0;
		float metallic = 0.0;
		DirectX::XMFLOAT4 color = { 0.0,0.0,0.0,0.0 };
	};

	struct ObjectCB
	{
		DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
	};

	struct RenderItem
	{
		UINT renderItemID;
		std::string name;
		ERenderItemType riType;
		
		Mesh mesh;

		VertexBuffer vb;
		IndexBuffer ib;

		UINT objectCBIndex;
		UINT materialCBIndex;
		ObjectCB objectCB;
		Material materialCB;

		CD3DX12_GPU_DESCRIPTOR_HANDLE textureHandle;

		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		EVertexLayout GetLayoutType();
	};

	class RenderItemManager
	{
	public:
		static RenderItemManager& GetInstance();
		static std::shared_ptr<RenderItem> GetRenderItem(UINT id);
		static std::shared_ptr<RenderItem> CreateRenderItem(std::string name, ERenderItemType riType);
		static void InitRenderItem(const Mesh& mesh, UINT id);
		static void InitRenderItem(const Mesh& mesh, ERenderItemType riType, UINT index);
		static void InitRenderItem(const Mesh& mesh, std::shared_ptr<RenderItem> ri);
	private:
		// std::string ValidifyName(std::string name);
	private:
		std::unordered_map<ERenderItemType,std::vector<std::shared_ptr<RenderItem>>> mRenderItems;
		std::unordered_map<UINT, std::pair< ERenderItemType, UINT>> mIdToItem;
		static int renderItemCount;
	};
}
