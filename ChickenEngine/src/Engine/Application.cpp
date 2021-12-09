#include "pch.h"

#include "Engine/Application.h"
#include <imgui/imgui_impl_dx12.h>

namespace ChickenEngine
{

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		s_Instance = this;
		Init();
	};

	Application::~Application()
	{
		ImguiManager::GetInstance().ImguiDestroy();
		mWindow->Shutdown();
	}

	void Application::Init()
	{

		// --------Init System--------
		// Init logger
		Log::GetCoreLogger()->set_level(spdlog::level::trace);
		Log::GetClientLogger()->set_level(spdlog::level::trace);

		DX12Renderer& renderer = DX12Renderer::GetInstance();
		// Init Window 
		mWindow = std::make_shared<Window>();
		mWindow->SetEventCallBack(BIND_FN(Application::OnEvent));
		LOG_INFO("INIT Wnd");
		mWindow->Init("Chicken Engine", 1280, 720);

		LOG_INFO("INIT DX12");
		if (!renderer.InitDX12(mWindow->MainWnd(), mWindow->GetWidth(), mWindow->GetHeight()))
		{
			LOG_ERROR("Initialize D3D fail");
			exit(1);
		}




		// --------Init Pipeline--------
		// Load Shader
		renderer.InitPipeline();

		renderer.OnResize(mWindow->GetWidth(), mWindow->GetHeight());
			// Init Imgui
		ImguiManager::GetInstance().ImguiInit();
	}
	

	void Application::Run()
	{
		mTimer.Reset();
		MSG msg = { 0 };

		// If there are Window messages then process them.
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			Update();

			Render();
		
			mTimer.Tick();

		}
	}

	void Application::Update()
	{
		// Update
		mWindow->Update();

	}

	void Application::Render()
	{
		// Prepare Cmdlist
		DX12Renderer::GetInstance().PrepareCommandList();

		//------------ foreach pass ----------------
		
		// ---- prepare ----
		// Set up viewport
		// Clear rtv,dsv
		// set render target, and transit the format
		// set root signature to use
		// 
		// ---- render ----
		// set parameter
		// set pso
		// render

		DX12Renderer::GetInstance().Draw();
		//Imgui
		ImguiManager::GetInstance().ImguiBegin();
		ImguiManager::GetInstance().ImguiRender(); // later be substituted
		ImguiManager::GetInstance().ImguiEnd();

		DX12Renderer::GetInstance().CloseCommandList();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_FN(Application::OnWindowClose));

		ImguiManager::GetInstance().OnEvent(e);
		
	}

	void Application::CalculateFrameStats()
	{
		static int frameCnt = 0;
		static float timeElapsed = 0.0f;

		frameCnt++;

		// Compute averages over one second period.
		if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
		{
			float fps = (float)frameCnt; // fps = frameCnt / 1
			float mspf = 1000.0f / fps;

			std::wstring fpsStr = std::to_wstring(fps);
			std::wstring mspfStr = std::to_wstring(mspf);

			//std::wstring windowText = "" +
			//	L"    fps: " + fpsStr +
			//	L"   mspf: " + mspfStr;

			//// DEBUG
			//SetWindowText(Window::MainWnd(), windowText.c_str());

			// Reset for next average.
			frameCnt = 0;
			timeElapsed += 1.0f;
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		//LOG_INFO("Inside win close event");
		m_Running = false;
		return true;
	}
	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		LOG_INFO("Window resize application");
		DX12Renderer::GetInstance().OnResize(e.GetWidth(), e.GetHeight());
		return false;
	}
}