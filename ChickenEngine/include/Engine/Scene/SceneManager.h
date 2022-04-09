#pragma once
#include "pch.h"
#include "Interface/IResource.h"
#include "Engine/Core.h"
#include "Helper/Singleton.h"
#include "Renderer/DX12Renderer/DX12Renderer.h"
#include "Camera.h"
#include "RenderObject.h"
#include "Model.h"

namespace ChickenEngine
{
	class CHICKEN_API SceneManager : public Singleton<SceneManager>
	{
	public:
		inline static Camera& GetCamera() { return instance().mCamera; }
		inline static std::deque< std::shared_ptr<RenderObject>>& GetAllRenderObjects(){ return instance().mRenderObjects; }
		static std::shared_ptr<RenderObject> GetRenderObject(uint32_t id);
		static std::shared_ptr<RenderObject> CreateRenderObject(std::string n);
		static std::shared_ptr<RenderObject> CreateRenderObject(std::string n, XMFLOAT3 p, XMFLOAT3 r, XMFLOAT3 s, XMFLOAT4 c, float ro, float me);
		static std::shared_ptr<RenderObject> CreateRenderObject(Mesh m, std::string n, XMFLOAT3 p, XMFLOAT3 r, XMFLOAT3 s, XMFLOAT4 c, float ro, float me);
		static std::shared_ptr<RenderObject> CreateRenderObject(Model m, std::string n, XMFLOAT3 p, XMFLOAT3 r, XMFLOAT3 s, XMFLOAT4 c, float ro, float me);
		static void LoadSkyBox();
		static void LoadDebugPlane();
		static void LoadAllRenderObjects(); // load to DX12
		
		static void BindSkyTex(Texture t);

		static void UpdateRenderObjects();
		static void UpdateSceneData(int width, int height);
		static void UpdateRenderSettings();
		static void ToggleRenderObjectVisibility(uint32_t id);

		void UpdateDirLightPosition();

		/* Light */
		inline static XMFLOAT3& GetDirLightRotation(){return instance().mDirLight.rotation;	}
		inline static XMFLOAT3  GetDirLightDirection() { return instance().mDirLight.data.direction; }
		inline static XMFLOAT3& GetDirLightStrength() { return instance().mDirLight.data.strength; }
		inline static XMFLOAT3& GetDirLightPosition() { return instance().mDirLight.position; }
		inline static bool&     GetDirLightAutoPos() { return instance().mDirLight.bAutoPosition; }
		inline static float&    GetDirLightDist() { return instance().mDirLight.distFrustumCenter; }
		inline static float&    GetDirLightOffset() { return instance().mDirLight.offsetViewDir; }
		inline static PassConstants& GetPassCB() { return instance().mPassCB; }
		inline static Camera& GetLightCamera() { return instance().mLightCamera; }
		/* Render Setting */
		inline static RenderSettings& GetRenderSettings() { return instance().mRenderSettings; }

	private:
		static std::vector<BYTE> GetSceneData(int width, int height);
		static void SetRenderObjectCB(RenderObject& ro); // called by UpdateRenderObjects/LoadAllRenderObjects
		static void UpdateDirLightDirection(); // called by UpdateSceneData
		static void UpdateLightCamera(); // called by UpdateSceneData

	private:
		Camera mCamera;
		Camera mLightCamera = Camera(true);
		DirectionLight mDirLight;
		PassConstants mPassCB;
		std::deque<std::shared_ptr<RenderObject>> mRenderObjects;
		RenderSettings mRenderSettings;

		static uint32_t renderObjectCount;
	};
}


