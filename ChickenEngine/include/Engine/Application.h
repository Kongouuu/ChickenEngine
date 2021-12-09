#pragma once

#include "Core.h"

#include "pch.h"
#include "Events/Event.h"
#include "Engine/Log.h"
#include "Events/ApplicationEvent.h"
#include "ImguiManager.h"
#include "Window.h"
#include "Renderer/DX12Renderer.h"
#include "GameTimer.h"

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
		void PollMsg();
		void Update();
		void Render();
		void OnEvent(Event& e);

		inline std::shared_ptr<Window> GetWindow() { return mWindow; }
		inline static Application& Get() { return *s_Instance; }
	protected:
		std::shared_ptr<Window> mWindow;
		GameTimer mTimer;

		void CalculateFrameStats();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool m_Running = true;

	private:
		static Application* s_Instance;

	};

	Application* CreateApplication();
}

