#pragma once

#include "DX12CommonHeader.h"
#include <DirectXMath.h>

namespace ChickenEngine
{
	struct Vertex
	{
		Vertex() {}
		Vertex(
			DirectX::XMFLOAT3 p,
			DirectX::XMFLOAT3 n,
			DirectX::XMFLOAT3 t,
			DirectX::XMFLOAT3 c,
			DirectX::XMFLOAT2 t1) :
			pos(p), normal(n),tangent(t), color(c), texC(t1) {}

		
		DirectX::XMFLOAT3 pos = { 0.0,0.0,0.0 };
		DirectX::XMFLOAT3 normal = { 0.0,0.0,0.0 };
		DirectX::XMFLOAT3 tangent = { 0.0,0.0,0.0 };
		DirectX::XMFLOAT3 color = { 0.0,0.0,0.0 };
		DirectX::XMFLOAT2 texC = { 0.0,0.0 };

	};

	struct VertexPNT
	{
		VertexPNT() {}
		VertexPNT(
			DirectX::XMFLOAT3 p,
			DirectX::XMFLOAT3 n,
			DirectX::XMFLOAT2 t) :
			pos(p), normal(n), texC(t) {}

		DirectX::XMFLOAT3 pos = { 0.0,0.0,0.0 };
		DirectX::XMFLOAT3 normal = { 0.0,0.0,0.0 };
		DirectX::XMFLOAT2 texC = { 0.0,0.0 };
	};

	struct VertexPNCT
	{
		VertexPNCT(
			DirectX::XMFLOAT3 p,
			DirectX::XMFLOAT3 n,
			DirectX::XMFLOAT3 c,
			DirectX::XMFLOAT2 t) :
			pos(p), normal(n), color(c), texC(t) {}

		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 color;
		DirectX::XMFLOAT2 texC;
	};

	struct Mesh
	{
		EVertexLayout layout;
		std::vector<Vertex> vertices;
		std::vector<UINT> indices;
	};

	struct MeshManager
	{
	public:
		static MeshManager& GetInstance();
		Mesh GenerateBox();
	};
}

