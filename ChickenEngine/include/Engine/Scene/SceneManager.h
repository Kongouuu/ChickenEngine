#pragma once
#include "pch.h"
#include "Engine/Core.h"
#include "Camera.h"
#include "Light.h"
#include "PassConstants.h"
#include "RenderObject.h"
#include "Helper/Singleton.h"

namespace ChickenEngine
{
	class CHICKEN_API SceneManager : public Singleton<SceneManager>
	{
	public:
		inline static Camera& GetCamera() { return instance().mCamera; }
		inline static PassConstants& GetPassCB() { return instance().mPassCB; }
		inline static std::deque< std::shared_ptr<RenderObject>>& GetAllRenderObjects(){ return instance().mRenderObjects; }
		static std::shared_ptr<RenderObject> GetRenderObject(UINT id);
		static std::shared_ptr<RenderObject> CreateRenderObject(std::string n);
		static std::shared_ptr<RenderObject> CreateRenderObject(std::string n, XMFLOAT3 p, XMFLOAT3 r, XMFLOAT3 s, XMFLOAT4 c, float ro, float me);
		static std::vector<BYTE> GetSceneData(int width, int height);

	private:
		Camera mCamera;
		DirectionLight mDirLight;
		PassConstants mPassCB;
		std::deque<std::shared_ptr<RenderObject>> mRenderObjects;
	};
}


