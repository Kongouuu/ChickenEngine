#pragma once

#include "Helper/DX12CommonHeader.h"
#include "Buffer.h"
#include "Texture.h"
#include "DescriptorHeap.h"
#include "FrameResource.h"

namespace ChickenEngine
{
	/* TMP */


	enum ERenderItemType
	{
		RI_OPAQUE = 0,
		RI_TRANSPARENT,
		RI_COUNT
	};

	struct CHICKEN_API RenderItem
	{
		UINT renderItemID = 0;
		std::string name = "";
		ERenderItemType riType = RI_OPAQUE;

		UINT indexCount = 0;
		VertexBuffer vb;
		IndexBuffer ib;
		UINT texOffset;

		int numFramesDirty = 0;


		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		EVertexLayout GetLayoutType();
		void Init(UINT vertexCount, size_t vertexSize, BYTE* vertexData, std::vector<uint16_t> indices);
		//void AddTexture(std::string name);


		// Debug usages
		int debugNum = 0;
	};

	class CHICKEN_API RenderItemManager : public Singleton<RenderItemManager>
	{
	public:
		
		static std::shared_ptr<RenderItem> GetRenderItem(UINT id);
		static std::shared_ptr<RenderItem> CreateRenderItem(ERenderItemType riType);

		inline static std::vector<std::shared_ptr<RenderItem>>& GetAllRenderItems() { return instance().mRenderItems; }
		inline static int RenderItemCount() { return renderItemCount; }

		static void UpdateAllRenderItemCB();
		//static void InitRenderItem(const Mesh& mesh, UINT id);
		//static void InitRenderItem(const Mesh& mesh, ERenderItemType riType, UINT index);
		//static void InitRenderItem(const Mesh& mesh, std::shared_ptr<RenderItem> ri);
	private:
		// std::string ValidifyName(std::string name);
	private:
		//std::unordered_map<ERenderItemType,std::vector<std::shared_ptr<RenderItem>>> mRenderItems;
		std::vector<std::shared_ptr<RenderItem>> mRenderItems;
		//std::vector < std::vector<std::shared_ptr<RenderItem>>> mRenderItemOfType[ERenderItemType::RI_COUNT];
		//std::unordered_map<UINT, std::pair< ERenderItemType, UINT>> mIdToItem;
		static int renderItemCount;
	};
}
