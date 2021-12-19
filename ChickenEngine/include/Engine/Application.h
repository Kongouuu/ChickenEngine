//#pragma once

#include "Core.h"

#include "pch.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Log.h"
#include "ImguiManager.h"
#include "Window.h"
#include "GameTimer.h"
#include "Scene/Camera.h"
#include "Renderer/DX12Renderer.h"
#include "ResourceManager.h"
#include "Helper/DX12Defines.h"
#include "Helper/MouseButtonEnum.h"
#include "Scene/PassConstants.h"
#include "Scene/SceneManager.h"

namespace ChickenEngine
{
	// Use declspec so that our main application can load from this dll
	class CHICKEN_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Init();
		void Run();
		void Update();
		void Render();
		void OnEvent(Event& e);

		// Init
		virtual void LoadTextures();
		virtual void LoadScene();
		virtual void InitCamera();

		// Loop Update
		virtual void UpdateCamera();

		// Called Update
		void SetSceneData();
		void SetRenderObjectTransform(RenderObject& ro);
		void SetRenderObjectMaterial(RenderObject& ro);
		void SetRenderObjectTexture(RenderObject& ro);
		int CreateRenderItem(std::string name, Mesh m, EVertexLayout layout = EVertexLayout::POS_NORM_TEX);

		inline std::shared_ptr<Window> GetWindow() { return mWindow; }
		inline static Application& Get() { return *s_Instance; }
	protected:
		std::shared_ptr<Window> mWindow;
		GameTimer mTimer;
		Camera mCamera;

		std::array<bool, 255> mKeyDown;
		std::array<bool, 5> mMouseDown;
		float previousMouseX;
		float previousMouseY;
		void CalculateFrameStats();

		// Return value decides whether event is handled or not
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnMouseMove(MouseMovedEvent& e);
		bool OnMousePressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool m_Running = true;

	private:
		static Application* s_Instance;
		
	};

	Application* CreateApplication();
}

