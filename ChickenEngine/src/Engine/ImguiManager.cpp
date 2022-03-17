#include "pch.h"
#include "Engine/ImguiManager.h"
#include "Engine/Application.h"
#include "Renderer/DX12Renderer/DX12Renderer.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, uint32_t msg, WPARAM wParam, LPARAM lParam);
namespace ChickenEngine
{
	int ImguiManager::IMcount = 0;
	ImguiManager::ImguiManager()
	{
		bEnabled = false;
	}

	ImguiManager::~ImguiManager()
	{
		
	}
	
	void ImguiManager::ImguiInit()
	{
		LOG_ERROR("Imgui Init");
		this->bEnabled = true; 
		ImGui::DebugCheckVersionAndDataLayout(IMGUI_VERSION, sizeof(ImGuiIO), sizeof(ImGuiStyle), sizeof(ImVec2), sizeof(ImVec4), sizeof(ImDrawVert), sizeof(ImDrawIdx));
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); 
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		io.KeyMap[ImGuiKey_Tab] = VK_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
		io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
		io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
		io.KeyMap[ImGuiKey_Home] = VK_HOME;
		io.KeyMap[ImGuiKey_End] = VK_END;
		io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
		io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
		io.KeyMap[ImGuiKey_Space] = VK_SPACE;
		io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
		io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
		io.KeyMap[ImGuiKey_KeyPadEnter] = VK_RETURN;

		io.KeyMap[ImGuiKey_A] = 'A';
		io.KeyMap[ImGuiKey_C] = 'C';
		io.KeyMap[ImGuiKey_V] = 'V';
		io.KeyMap[ImGuiKey_X] = 'X';
		io.KeyMap[ImGuiKey_Y] = 'Y';
		io.KeyMap[ImGuiKey_Z] = 'Z';
		
		io.Fonts->Build();


		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& App = Application::Get();
		const DX12Renderer& DXContext = DX12Renderer::GetInstance();
	
		ImGui_ImplWin32_Init(App.GetWindow()->MainWnd());
		ImGui_ImplDX12_Init(DXContext.D3dDevice().Get(), NUM_FRAMES_IN_FLIGHT,
			DXGI_FORMAT_R8G8B8A8_UNORM, DXContext.SrvHeap().Get(),
			DXContext.SrvHeap()->GetCPUDescriptorHandleForHeapStart(),
			DXContext.SrvHeap()->GetGPUDescriptorHandleForHeapStart());
		LOG_INFO("After Imgui Init");

		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		
	}

	void ImguiManager::ImguiDestroy()
	{
		LOG_ERROR("destroyed imgui");
		// Cleanup
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		bEnabled = false;

	}

	bool ImguiManager::ImguiMsgProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
	{
		return ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);		
	}

	void ImguiManager::ImguiBegin()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void ImguiManager::ImguiRender()
	{
		ImguiBegin();

		ShowDockSpace();
		ShowScenePanel();
		ShowLightPanel();
		ShowViewPortPanel();
		ShowSettingsPanel();
		ShowStatsPanel();
		ImguiEnd();
	}

	void ImguiManager::ImguiEnd()
	{
		Application& App = Application::Get();
		const DX12Renderer& DXContext = DX12Renderer::GetInstance();
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(App.GetWindow()->GetWidth(), App.GetWindow()->GetHeight());

		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DXContext.CommandList().Get());

		// If using imgui docking/viewport, has to do this for update
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault(NULL, (void*)DXContext.CommandList().Get());
		}
	}

	void ImguiManager::OnEvent(Event& event)
	{
		if (!bEnabled)
			return;
		ImGuiIO& io = ImGui::GetIO();
		event.Handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
		event.Handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
	}

	void ImguiManager::ShowDockSpace()
	{
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	}

	void ImguiManager::ShowViewPortPanel()
	{
		ImGui::Begin("ViewPort Panel");
		ImGuiIO& io = ImGui::GetIO();
		// Focus在viewport的时候才会传控制给application，不然就截断消息
		io.WantCaptureMouse = 1 ^ ImGui::IsWindowFocused();
		io.WantCaptureKeyboard = 1 ^ ImGui::IsWindowFocused();

		// 这个函数可以得到当前UI的大小
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		// 如果UI有resize，那么标记为脏
		if (abs(mViewportWidth - viewportPanelSize.x) >= 0.1 || abs(mViewportHeight - viewportPanelSize.y) >= 0.1)
		{
			mViewportWidth = viewportPanelSize.x;
			mViewportHeight = viewportPanelSize.y;
			mViewportSizeDirty = true;
		}

		// 渲染贴图
		ImGui::Image((ImTextureID)(DX12Renderer::GetInstance().CurrentViewportBufferGPUHandle().ptr), viewportPanelSize, ImVec2{ 0, 0 }, ImVec2{ 1, 1 });
		ImGui::End();
	}

	void ImguiManager::ShowScenePanel()
	{
		ImGui::Begin("Scene Panel");

		std::deque<std::shared_ptr<RenderObject>>& renderObjs = SceneManager::GetAllRenderObjects();
		for (auto& ro : renderObjs)
		{
			if (ImGui::TreeNode(ro->name.c_str()))
			{
				ImGui::Text("Render Object id: %u",ro->renderObjectID);

				if (ro->fixed == false)
				{
					// Position
					float posx = ro->position.x;
					float posy = ro->position.y;
					float posz = ro->position.z;
					ImGui::Text("Local Position:");
					IMGUI_LEFT_LABEL(ImGui::DragFloat, "pos x: ", &ro->position.x, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
					IMGUI_LEFT_LABEL(ImGui::DragFloat, "pos y: ", &ro->position.y, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
					IMGUI_LEFT_LABEL(ImGui::DragFloat, "pos z: ", &ro->position.z, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
					if (posx != ro->position.x || posy != ro->position.y || posz != ro->position.z)
						ro->dirty = true;

					// Rotation
					float rotx = ro->rotation.x;
					float roty = ro->rotation.y;
					float rotz = ro->rotation.z;
					ImGui::Text("Rotation:");
					IMGUI_LEFT_LABEL(ImGui::DragFloat, "rot x: ", &ro->rotation.x, 0.02f, -FLT_MAX, +FLT_MAX, "%.3f");
					IMGUI_LEFT_LABEL(ImGui::DragFloat, "rot y: ", &ro->rotation.y, 0.02f, -FLT_MAX, +FLT_MAX, "%.3f");
					IMGUI_LEFT_LABEL(ImGui::DragFloat, "rot z: ", &ro->rotation.z, 0.02f, -FLT_MAX, +FLT_MAX, "%.3f");
					if (rotx != ro->rotation.x || roty != ro->rotation.y || rotz != ro->rotation.z)
						ro->dirty = true;

					// Scale
					float scax = ro->scale.x;
					float scay = ro->scale.y;
					float scaz = ro->scale.z;
					ImGui::Text("Scale:");
					IMGUI_LEFT_LABEL(ImGui::DragFloat, "sca x: ", &ro->scale.x, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
					IMGUI_LEFT_LABEL(ImGui::DragFloat, "sca y: ", &ro->scale.y, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
					IMGUI_LEFT_LABEL(ImGui::DragFloat, "sca z: ", &ro->scale.z, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
					if (scax != ro->scale.x || scay != ro->scale.y || scaz != ro->scale.z)
						ro->dirty = true;

					ImGui::Text("Material:");
					// Roughness
					float roughness = ro->roughness;
					IMGUI_LEFT_LABEL(ImGui::DragFloat, "roughness: ", &ro->roughness, 0.003f, 0.0f, 1.0f, "%.3f");
					// Metallic
					float metallic = ro->metallic;
					IMGUI_LEFT_LABEL(ImGui::DragFloat, "metallic:  ", &ro->metallic, 0.003f, 0.0f, 1.0f, "%.3f");
					if (roughness != ro->roughness || metallic != ro->metallic)
						ro->dirty = true;
				}

				ImGui::Text("Visibility:");
				bool visible = ro->visible;
				IMGUI_LEFT_LABEL(ImGui::Checkbox, "visible:", &ro->visible);
				if (visible != ro->visible)
					SceneManager::ToggleRenderObjectVisibility(ro->renderObjectID);
	
				ImGui::TreePop();
				ImGui::Separator();
			}
		}
		
		ImGui::End();
	}

	void ImguiManager::ShowDetailPanel()
	{
	}

	void ImguiManager::ShowCameraPanel()
	{
	}

	void ImguiManager::ShowLightPanel()
	{
		ImGui::Begin("Light Panel");
		if(ImGui::TreeNode("Direction Light"))
		{
			DirectX::XMFLOAT3 dir = SceneManager::GetDirLightDirection();
			DirectX::XMFLOAT3& str = SceneManager::GetDirLightStrength();
			DirectX::XMFLOAT3& rot = SceneManager::GetDirLightRotation();
			DirectX::XMFLOAT3& pos = SceneManager::GetDirLightPosition();
			bool& bAutoPos = SceneManager::GetDirLightAutoPos();
			float& distFrustum = SceneManager::GetDirLightDist();
			float& offsetViewDir = SceneManager::GetDirLightOffset();

			ImGui::Text("Direction: ");
			ImGui::Text("%.2f  %.2f  %.2f", dir.x, dir.y, dir.z);
			ImGui::Text("Strength: ");
			IMGUI_LEFT_LABEL(ImGui::DragFloat, "r: ", &str.x, 0.2f, 0.0, +FLT_MAX, "%.3f");
			IMGUI_LEFT_LABEL(ImGui::DragFloat, "g: ", &str.y, 0.2f, 0.0, +FLT_MAX, "%.3f");
			IMGUI_LEFT_LABEL(ImGui::DragFloat, "b: ", &str.z, 0.2f, 0.0, +FLT_MAX, "%.3f");

			ImGui::Text("Rotation: ");
			IMGUI_LEFT_LABEL(ImGui::DragFloat, "rot x: ", &rot.x, 0.2f, -FLT_MAX, +FLT_MAX, "%.3f");
			IMGUI_LEFT_LABEL(ImGui::DragFloat, "rot y: ", &rot.y, 0.2f, -FLT_MAX, +FLT_MAX, "%.3f");
			IMGUI_LEFT_LABEL(ImGui::DragFloat, "rot z: ", &rot.z, 0.2f, -FLT_MAX, +FLT_MAX, "%.3f");

			ImGui::Text("Position: ");
			ImGui::Text("%.2f  %.2f  %.2f", pos.x, pos.y, pos.z);
			static bool autoPos = bAutoPos;
			IMGUI_LEFT_LABEL(ImGui::Checkbox, "Auto Adjust Position", &autoPos);
			bAutoPos = autoPos;
			if (bAutoPos)
			{
				ImGui::Text("Distance From Frustum: ");
				ImGui::DragFloat("##", &distFrustum, 1.0f, -FLT_MAX, +FLT_MAX, "%.3f");
				ImGui::Text("Offset View Direction :");
				ImGui::DragFloat("####", &offsetViewDir, 1.0f, -FLT_MAX, +FLT_MAX, "%.3f");
				//IMGUI_LEFT_LABEL(ImGui::DragFloat, "Distance From Frustum: ", &distFrustum, 1.0f, -FLT_MAX, +FLT_MAX, "%.3f");
				//IMGUI_LEFT_LABEL(ImGui::DragFloat, "Offset View Direction: ", &offsetViewDir, 1.0f, -FLT_MAX, +FLT_MAX, "%.3f");
			}
			else
			{
				IMGUI_LEFT_LABEL(ImGui::DragFloat, "pos x: ", &pos.x, 0.2f, -FLT_MAX, +FLT_MAX, "%.3f");
				IMGUI_LEFT_LABEL(ImGui::DragFloat, "pos y: ", &pos.y, 0.2f, -FLT_MAX, +FLT_MAX, "%.3f");
				IMGUI_LEFT_LABEL(ImGui::DragFloat, "pos z: ", &pos.z, 0.2f, -FLT_MAX, +FLT_MAX, "%.3f");
			}



			ImGui::TreePop();
			ImGui::Separator();
		}
		ImGui::End();
	}

	void ImguiManager::ShowSettingsPanel()
	{
		ImGui::Begin("Settings Panel");
		if (ImGui::TreeNode("Shadow"))
		{
			RenderSettings& rs = SceneManager::GetRenderSettings();
			static bool bEnabledSM;
			IMGUI_LEFT_LABEL(ImGui::Checkbox, "Generate Shadow Map", &bEnabledSM);
			rs.sm_generateSM = bEnabledSM;
			if (!bEnabledSM)
			{
				rs.sm_type = static_cast<int>(EShadowType::SM_DISABLED);
				ImGui::BeginDisabled();
			}
			Camera& lightCam = SceneManager::GetLightCamera();
			static float size = lightCam.GetFarWindowHeight();
			IMGUI_LEFT_LABEL(ImGui::DragFloat, "Frustum Size ", &size, 1.0f, 10.0f, +FLT_MAX, "%.3f");

			static float farZ = lightCam.GetFarZ();
			IMGUI_LEFT_LABEL(ImGui::DragFloat, "Far Z ", &farZ, 1.0f, 20.0f , +FLT_MAX, "%.3f");

			if (farZ!= lightCam.GetFarZ() || size != lightCam.GetFarWindowHeight())
			{
				lightCam.SetLensOrtho(size, size, lightCam.GetNearZ(), farZ);
			}

			ImGui::Text("Shadow Type: ");
			static int select = -1;
			IMGUI_LEFT_LABEL(ImGui::RadioButton, "Disabled", &select, 0);
			IMGUI_LEFT_LABEL(ImGui::RadioButton, "Default", &select,1);
			IMGUI_LEFT_LABEL(ImGui::RadioButton, "PCF", &select, 2);
			IMGUI_LEFT_LABEL(ImGui::RadioButton, "PCSS", &select, 3);
			IMGUI_LEFT_LABEL(ImGui::RadioButton, "VSSM", &select, 4);
			if (select >= 0)
			{
				rs.sm_type = select;
			}

			static float vsmVar;
			IMGUI_LEFT_LABEL(ImGui::DragFloat, "VSM var ", &vsmVar, 0.01f, 0.0f, +FLT_MAX, "%.9f");
			SceneManager::GetPassCB().vsmMinVariance = vsmVar;

			static float vsmAmount;
			IMGUI_LEFT_LABEL(ImGui::DragFloat, "VSM amount ", &vsmAmount, 0.01f, 0.0f, +FLT_MAX, "%.9f");
			SceneManager::GetPassCB().vsmAmount = vsmAmount;

			if (!bEnabledSM)
			{
				ImGui::EndDisabled();
			}

			ImGui::TreePop();
			ImGui::Separator();
		}
		ImGui::End();
	}

	void ImguiManager::ShowStatsPanel()
	{
		ImGui::Begin("Stats Panel");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

}