#pragma once

#include "Helper/DX12CommonHeader.h"
#include "Renderer/Buffer.h"
#include "Renderer/Texture.h"
#include "Renderer/DescriptorHeap.h"
#include "Mesh.h"

namespace ChickenEngine
{
	/* TMP */
	struct CHICKEN_API ObjectConstants
	{
		DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
	};

	struct CHICKEN_API PassConstants
	{
		DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
		float cbPerObjectPad1 = 0.0f;
		DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
		DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
		float NearZ = 0.0f;
		float FarZ = 0.0f;
	};
	/* TMP */

	enum ERenderItemType
	{
		RI_OPAQUE = 0,
		RI_TRANSPARENT,
		RI_OTHER
	};

	struct CHICKEN_API Material
	{
		Material() {}
		Material(float _roughness, float _metallic) : roughness(_roughness), metallic(_metallic){}
		void SetColor(DirectX::XMFLOAT4 _color) { color = _color; }
		float roughness = 0.0;
		float metallic = 0.0;
		DirectX::XMFLOAT4 color = { 0.0,0.0,0.0,0.0 };
	};

	struct CHICKEN_API RenderItem
	{
		UINT renderItemID;
		std::string name;
		ERenderItemType riType;
		
		Mesh mesh;

		UINT indexCount;
		VertexBuffer vb;
		IndexBuffer ib;


		UINT objectCBIndex;
		UINT materialCBIndex;
		ObjectConstants objectCB;
		Material materialCB;

		CD3DX12_GPU_DESCRIPTOR_HANDLE textureHandle;

		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		EVertexLayout GetLayoutType();
		void Init(const Mesh& _mesh);
		void AddTexture(std::string name);

	};

	class CHICKEN_API RenderItemManager : public Singleton<RenderItemManager>
	{
	public:
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
