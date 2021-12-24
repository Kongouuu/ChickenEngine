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
#pragma region Basic_Procedure
	void Application::Init()
	{
		// --------Init System--------
		// Init logger
		Log::GetCoreLogger()->set_level(spdlog::level::trace);
		Log::GetClientLogger()->set_level(spdlog::level::trace);

		mKeyDown.fill(false);
		mMouseDown.fill(false);

		// Init Window 
		mWindow = std::make_shared<Window>();
		mWindow->SetEventCallBack(BIND_FN(Application::OnEvent));
		mWindow->Init("Chicken Engine", 1280, 720);

		DX12Renderer& renderer = DX12Renderer::GetInstance();
		if (!renderer.InitDX12(mWindow->MainWnd(), mWindow->GetWidth(), mWindow->GetHeight()))
		{
			LOG_ERROR("Initialize D3D fail");
			exit(1);
		}
		

		// --------Init Pipeline--------
		renderer.PreInputAssembly();
		renderer.SetPassCBByteSize(sizeof(PassConstants));
		renderer.SetObjectCBByteSize(sizeof(ObjectConstants));
		renderer.SetMaterialCBByteSize(sizeof(MaterialConstants));
		renderer.StartDirectCommands();
		LoadTextures();
		LoadScene();
		renderer.CreateFrameResources(numFrameResources);
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
		mWindow->Update();
		UpdateCamera();
		SetSceneData();
		UpdateRenderObjects();
		DX12Renderer::GetInstance().UpdateFrame();
	}

	void Application::Render()
	{
		DX12Renderer& renderer = DX12Renderer::GetInstance();


		renderer.PrepareDraw();
		renderer.BindAllMapToNull();
		std::deque<std::shared_ptr<RenderObject>>& renderObjects =  SceneManager::GetAllRenderObjects();
		for (auto& ro : renderObjects)
		{
			renderer.BindObjectCB(ro->objectCBOffset);
			renderer.BindMaterialCB(ro->materialCBOffset);
			for (auto& mesh : ro->mMeshes)
			{
				if(mesh.diffuseMap.id >=0)
					renderer.BindDiffuseMap(mesh.diffuseMap.id);
				renderer.DrawRenderItem(mesh.renderItemID);
			}
		}

		ImguiManager::GetInstance().ImguiRender(); // later be substituted

		renderer.EndDraw();
	}

#pragma endregion Basic_Procedure

#pragma region PipeLine_Init
	void Application::InitCamera()
	{
		SceneManager::GetCamera().SetPosition({ 0.0,0.0,-3.0 });
		SceneManager::GetCamera().UpdateViewMatrix();
	}


	void Application::LoadTextures()
	{
		std::string filePath = FileHelper::GetTexturePath("timg.jpg");
		ResourceManager::LoadTexture(filePath, "tutou");
	}

	void Application::LoadScene()
	{
		InitCamera();

		LOG_TRACE("Enter Load Scene");
		DX12Renderer& renderer = DX12Renderer::GetInstance();

		//Add simple object BOX!
		std::shared_ptr<RenderObject> ro = SceneManager::CreateRenderObject(std::string("cube"), { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,1.0,1.0 }, { 1.0,1.0,1.0, 1.0 }, 0.15, 0.04);
		Mesh cube = MeshManager::GenerateBox();
		cube.AddDiffuseTexture(ResourceManager::GetTexture("tutou"));
		ro->mMeshes.push_back(cube);
		LoadRenderObject(ro);
		SetRenderObjectTransform(*ro);
		SetRenderObjectMaterial(*ro);


		// Load model
		Model m;
		m.LoadModel(FileHelper::GetModelPath("qiuqiuren/qiuqiuren.pmx"));
		std::shared_ptr<RenderObject> ro1 = SceneManager::CreateRenderObject(std::string("qiuqiuren"), { 0.0,0.0,0.0 }, { 0.0,180.0,0.0 }, { 0.1,0.1,0.1 }, { 1.0,1.0,1.0, 1.0 }, 0.15, 0.04);
		ro1->mMeshes.insert(ro1->mMeshes.end(), m.mMeshes.begin(), m.mMeshes.end());
		LoadRenderObject(ro1);
		SetRenderObjectTransform(*ro1);
		SetRenderObjectMaterial(*ro1);
	}

	void Application::LoadRenderObject(std::shared_ptr<RenderObject> ro)
	{
		ro->objectCBOffset = DX12Renderer::GetInstance().AddObjectCB();
		ro->materialCBOffset = DX12Renderer::GetInstance().AddMaterialCB();
		for (auto& m : ro->mMeshes)
		{
			BYTE* data = reinterpret_cast<BYTE*>(m.vertices.data());
			m.renderItemID =  DX12Renderer::GetInstance().CreateRenderItem( m.vertices.size(), sizeof(Vertex), data, m.GetIndices16());
		}
		
	}
#pragma endregion PipeLine_Init

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

	void Application::UpdateRenderObjects()
	{
		std::deque<std::shared_ptr<RenderObject>>& renderObjs = SceneManager::GetAllRenderObjects();
		for (auto& ro : renderObjs)
		{
			if (ro->dirty)
			{
				SetRenderObjectMaterial(*ro);
				SetRenderObjectTransform(*ro);
				ro->dirty = false;
			}
		}
	}
#pragma endregion Update

#pragma region Renderer_Communication
	void Application::SetSceneData()
	{
		SceneManager::UpdateDirLightDirection();
		std::vector<BYTE> data = SceneManager::GetSceneData(mWindow->GetWidth(), mWindow->GetHeight());
		DX12Renderer::GetInstance().SetPassSceneData(data.data());
	}

	void Application::SetRenderObjectTransform(RenderObject& ro)
	{
		XMMATRIX world;
		ObjectConstants oc;
		XMFLOAT3 rotationPI = ro.rotation;
		rotationPI.x *= (DirectX::XM_PI / 180.0f);
		rotationPI.y *= (DirectX::XM_PI / 180.0f);
		rotationPI.z *= (DirectX::XM_PI / 180.0f);
		world = XMMatrixTranslationFromVector(XMLoadFloat3(&ro.position)) *
			(XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rotationPI)) * XMMatrixScalingFromVector(XMLoadFloat3(&ro.scale)));
		XMStoreFloat4x4(&oc.World, XMMatrixTranspose(world));
		BYTE data[sizeof(ObjectConstants)];
		memcpy(&data, &oc, sizeof(ObjectConstants));
		
		DX12Renderer::GetInstance().SetObjectCB(ro.objectCBOffset, data);
	}

	void Application::SetRenderObjectMaterial(RenderObject& ro)
	{
		MaterialConstants m;
		m.Roughness = ro.roughness;
		m.Metallic = ro.metallic;
		m.Color = ro.color;

		BYTE data[sizeof(MaterialConstants)];
		memcpy(&data, &m, sizeof(MaterialConstants));

		DX12Renderer::GetInstance().SetMaterialCB(ro.materialCBOffset, data);
	}

	//void Application::SetRenderObjectTexture(RenderObject& ro)
	//{
	//	DX12Renderer::GetInstance().SetRenderItemTexture(ro.renderItemID, ro.texID);
	//}

#pragma endregion Renderer_Communication

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