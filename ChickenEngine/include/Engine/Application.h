//#pragma once

#include "Core.h"
#include "pch.h"
#include "Events/Event.h"
#include "GameLayer.h"
#include "Events/ApplicationEvent.h"
#include "Log.h"
#include "ImguiManager.h"
#include "Window.h"
#include "GameTimer.h"
#include "Scene/Camera.h"
#include "Renderer/DX12Renderer/DX12Renderer.h"
#include "ResourceManager.h"
#include "Helper/MouseButtonEnum.h"
#include "Scene/SceneManager.h"
#include "Scene/Model.h"

namespace ChickenEngine
{
	// Use declspec so that our main application can load from this dll
	class CHICKEN_API Application
	{
	public:
		Application();
		virtual ~Application();
		virtual void Init();
		void Run();
		void Update();
		void Render();
		void OnEvent(Event& e);

		// Attach specific game app
		void AttachGameLayer(GameLayer* _gl) { gl = _gl; }

		// temp
		void UpdateCamera();

		inline std::shared_ptr<Window> GetWindow() { return mWindow; }
		inline static Application& Get() { return *s_Instance; }
	protected:
		std::shared_ptr<Window> mWindow;
		GameTimer mTimer;
		Camera mCamera;
		GameLayer* gl;

		UINT numFrameResources = 3;
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

