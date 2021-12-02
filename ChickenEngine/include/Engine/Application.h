#pragma once

#include "Core.h"

#include "pch.h"
#include "Events/Event.h"
#include "Engine/Log.h"
#include "Events/ApplicationEvent.h"
#include "ImguiManager.h"
#include "LayerStack.h"
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

		void PushLayer(std::shared_ptr<Layer> layer);
		void PushOverlay(std::shared_ptr<Layer> layer);
		void PopLayer(std::shared_ptr<Layer> layer);

		inline std::shared_ptr<Window> GetWindow() { return mWindow; }
		inline static Application& Get() { return *s_Instance; }
	protected:
		std::shared_ptr<Window> mWindow;
		GameTimer mTimer;

		void CalculateFrameStats();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool m_Running = true;
		LayerStack mLayerStack;
	private:
		static Application* s_Instance;

	};

	Application* CreateApplication();
}

