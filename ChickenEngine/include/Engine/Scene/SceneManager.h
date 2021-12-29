#pragma once
#include "pch.h"
#include "Engine/Core.h"
#include "Camera.h"
#include "Interface/IResource.h"
#include "RenderObject.h"
#include "Model.h"
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
		static std::shared_ptr<RenderObject> CreateRenderObject(Mesh m, std::string n, XMFLOAT3 p, XMFLOAT3 r, XMFLOAT3 s, XMFLOAT4 c, float ro, float me);
		static std::shared_ptr<RenderObject> CreateRenderObject(Model m, std::string n, XMFLOAT3 p, XMFLOAT3 r, XMFLOAT3 s, XMFLOAT4 c, float ro, float me);
		static std::vector<BYTE> GetSceneData(int width, int height);

		static void UpdateDirLightDirection();
		static void LoadAllRenderObjects();
		static void UpdateRenderObjects();
		static void UpdateLightCamera();
		static void UpdateSceneData(int width, int height);

		static void SetRenderObjectCB(RenderObject& ro);

		inline static XMFLOAT3& GetDirLightRotation(){return instance().mDirLight.Rotation;	}
		inline static XMFLOAT3 GetDirLightDirection() { return instance().mDirLight.data.Direction; }
		inline static XMFLOAT3& GetDirLightStrength() { return instance().mDirLight.data.Strength; }
	private:
		Camera mCamera;
		Camera mLightCamera;
		DirectionLight mDirLight;
		PassConstants mPassCB;
		std::deque<std::shared_ptr<RenderObject>> mRenderObjects;

		static UINT renderObjectCount;
	};
}


