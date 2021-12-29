#include "pch.h"

#include "Engine/Application.h"
#include <imgui/imgui_impl_dx12.h>

namespace ChickenEngine
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		s_Instance = this;
	};

	Application::~Application()
	{
		ImguiManager::GetInstance().ImguiDestroy();
		mWindow->Shutdown();
	}

#pragma region Basic_Procedure
	void Application::Init()
	{
		// --------Init Systems--------
		// Init logger
		Log::GetCoreLogger()->set_level(spdlog::level::trace);
		Log::GetClientLogger()->set_level(spdlog::level::trace);
		// Init keys
		mKeyDown.fill(false);
		mMouseDown.fill(false);
		// Init Window 
		mWindow = std::make_shared<Window>();
		mWindow->SetEventCallBack(BIND_FN(Application::OnEvent));
		mWindow->Init("Chicken Engine", 1280, 720);
		// Init dx12
		DX12Renderer& renderer = DX12Renderer::GetInstance();
		if (!renderer.InitDX12(mWindow->MainWnd(), mWindow->GetWidth(), mWindow->GetHeight(), 3))
		{
			LOG_ERROR("Initialize D3D fail");
			exit(1);
		}
		// --------Init pipeline--------
		renderer.StartDirectCommands();
		gl->LoadScene();
		Mesh debugPlane = ChickenEngine::MeshManager::GenerateDebugPlane();
		SceneManager::CreateRenderObject(debugPlane, std::string("debugPlane"), { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,1.0,1.0 }, { 0.0,0.0,0.0,0.0 }, 0.15, 0.04);


		SceneManager::LoadAllRenderObjects();
		renderer.CreateFrameResources();
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
			else
			{
				mTimer.Tick();
				int fps = static_cast<int>(1.f / mTimer.DeltaTime());
				//LOG_INFO("fps: {0}", fps);
				CalculateFrameStats();
				Update();
				Render();
			}
		}
	}

	void Application::Update()
	{
		// Update
		gl->Update();

		SceneManager::UpdateRenderObjects();
		UpdateCamera();
		SceneManager::UpdateSceneData(mWindow->GetWidth(), mWindow->GetHeight());
		
		DX12Renderer::GetInstance().UpdateFrame();
	}

	void Application::Render()
	{
		DX12Renderer& renderer = DX12Renderer::GetInstance();
		renderer.Render();
		// call actual render 
		ImguiManager::GetInstance().ImguiRender(); // later be substituted

		renderer.EndRender();
	}

#pragma endregion Basic_Procedure

#pragma region Update
	void Application::UpdateCamera()
	{
		Camera& camera = SceneManager::GetCamera();
		// Position
		if (mKeyDown[(int)'w'] || mKeyDown[(int)'W'])
			camera.Walk(10.0f * mTimer.DeltaTime());
		if (mKeyDown[(int)'s'] || mKeyDown[(int)'S'])
			camera.Walk(-10.0f * mTimer.DeltaTime());
		if (mKeyDown[(int)'d'] || mKeyDown[(int)'D'])
			camera.Strafe(10.0f * mTimer.DeltaTime());
		if (mKeyDown[(int)'a'] || mKeyDown[(int)'A'])
			camera.Strafe(-10.0f * mTimer.DeltaTime());

		// View direction
		camera.UpdateViewMatrix();
	}

#pragma endregion Update


#pragma region Util
	void Application::CalculateFrameStats()
	{
		static int frameCnt = 0;
		static float timeElapsed = 0.0f;
		static float preTime = 0.0;
		frameCnt++;
		// Compute averages over one second period.
		if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
		{
			float fps = (float)frameCnt; // fps = frameCnt / 1
			float mspf = 1000.0f / fps;
			//LOG_INFO("FPS: {0}", fps);
			std::wstring fpsStr = std::to_wstring(fps);
			std::wstring mspfStr = std::to_wstring(mspf);

		
			frameCnt = 0;
			timeElapsed += 1.0f;
		}
		preTime = mTimer.TotalTime();
	}
#pragma endregion Util

#pragma region EventHandle
	void Application::OnEvent(Event& e)
	{
		ImguiManager::GetInstance().OnEvent(e);
		if (e.Handled)
			return;
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_FN(Application::OnWindowResize));
		dispatcher.Dispatch<MouseMovedEvent>(BIND_FN(Application::OnMouseMove));
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_FN(Application::OnMousePressedEvent));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_FN(Application::OnMouseReleasedEvent));
		dispatcher.Dispatch<KeyPressedEvent>(BIND_FN(Application::OnKeyPressedEvent));
		dispatcher.Dispatch<KeyReleasedEvent>(BIND_FN(Application::OnKeyReleasedEvent));

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
		SceneManager::GetCamera().SetAspect(static_cast<float>(e.GetWidth()) / e.GetHeight());
		DX12Renderer::GetInstance().OnResize(e.GetWidth(), e.GetHeight());
		return false;
	}


	bool Application::OnMouseMove(MouseMovedEvent& e)
	{
		if (mMouseDown[EMouseButton::LEFT])
		{
			// Make each pixel correspond to a quarter of a degree.
			float dx = XMConvertToRadians(0.25f * static_cast<float>(e.GetX() - previousMouseX));
			float dy = XMConvertToRadians(0.25f * static_cast<float>(e.GetY() - previousMouseY));

			SceneManager::GetCamera().Pitch(dy);
			SceneManager::GetCamera().RotateY(dx);
		}

		previousMouseX = e.GetX();
		previousMouseY = e.GetY();
		return true;
	}

	bool Application::OnMousePressedEvent(MouseButtonPressedEvent& e)
	{
		previousMouseX = e.GetX();
		previousMouseY = e.GetY();
		mMouseDown[e.GetMouseButton()] = true;
		return true;
	}

	bool Application::OnMouseReleasedEvent(MouseButtonReleasedEvent& e)
	{
		mMouseDown[e.GetMouseButton()] = false;
		return true;
	}

	bool Application::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		mKeyDown[e.GetKeyCode()] = true;
		return true;
	}


	bool Application::OnKeyReleasedEvent(KeyReleasedEvent& e)
	{
		mKeyDown[e.GetKeyCode()] = false;
		return true;
	}
#pragma endregion EventHandle

}