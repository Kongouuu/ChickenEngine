﻿#pragma once

#include "Helper/DX12CommonHeader.h"
#include "Interface/IResource.h"
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
		uint32_t renderItemID = 0;
		std::string name = "";
		ERenderItemType riType = RI_OPAQUE;

		uint32_t indexCount = 0;
		VertexBuffer vb;
		IndexBuffer ib;
		D3D12_GPU_DESCRIPTOR_HANDLE diffuseHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE specularHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE normalHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE heightHandle;

		uint32_t cbOffset;
		int numFramesDirty = 0;

		bool visible = true;
		bool debug = false;
		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		void Init(uint32_t vertexCount, size_t vertexSize, BYTE* vertexData, std::vector<uint16_t> indices);
		//void AddTexture(std::string name);


		// Debug usages
		int debugNum = 0;
	};

	class CHICKEN_API RenderItemManager : public Singleton<RenderItemManager>
	{
	public:
		static std::shared_ptr<RenderItem> GetRenderItem(uint32_t id);
		static std::shared_ptr<RenderItem> CreateRenderItem(ERenderItemType riType);

		inline static std::vector<std::shared_ptr<RenderItem>>& GetAllRenderItems() { return instance().mRenderItems; }
		inline static int RenderItemCount() { return renderItemCount; }

		//static void InitRenderItem(const Mesh& mesh, uint32_t id);
		//static void InitRenderItem(const Mesh& mesh, ERenderItemType riType, uint32_t index);
		//static void InitRenderItem(const Mesh& mesh, std::shared_ptr<RenderItem> ri);
	private:
		// std::string ValidifyName(std::string name);
	private:
		//std::unordered_map<ERenderItemType,std::vector<std::shared_ptr<RenderItem>>> mRenderItems;
		std::vector<std::shared_ptr<RenderItem>> mRenderItems;
		std::shared_ptr<RenderItem> mDebugRenderItem;
		//std::vector < std::vector<std::shared_ptr<RenderItem>>> mRenderItemOfType[ERenderItemType::RI_COUNT];
		//std::unordered_map<UINT, std::pair< ERenderItemType, UINT>> mIdToItem;
		static int renderItemCount;
	};
}
