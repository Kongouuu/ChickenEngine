#pragma once

#include "Mesh.h"

namespace ChickenEngine
{
	using namespace DirectX;
	struct CHICKEN_API RenderObject
	{
		uint32_t renderObjectID = 0;
		ERenderLayer layer = ERenderLayer::L_DEFAULT;
		std::string name = "";
		XMFLOAT3 position;
		XMFLOAT3 rotation ;
		XMFLOAT3 scale;
		XMFLOAT4 color;
		float roughness = 0.0;
		float metallic = 0.0;
		bool visible = true;
		bool fixed = false;
		bool autoLoad = true;
		bool dirty = false;
		std::vector<Mesh> mMeshes;


		RenderObject(std::string n) : name(n) {
			position = XMFLOAT3(0.0, 0.0, 0.0);
			rotation = XMFLOAT3(0.0, 0.0, 0.0);
			scale = XMFLOAT3(1.0, 1.0, 1.0);
			color = XMFLOAT4(1.0, 0.0, 0.0, 1.0);
		}

		RenderObject(std::string n, XMFLOAT3 p, XMFLOAT3 r, XMFLOAT3 s, XMFLOAT4 c, float ro, float me)
		{
			name = n;
			position = p;
			rotation = r;
			scale = s;
			color = c;
			roughness = ro;
			metallic = me;
		}
	};
}