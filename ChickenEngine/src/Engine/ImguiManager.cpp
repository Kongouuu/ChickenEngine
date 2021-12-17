#include "pch.h"
#include "Engine/ImguiManager.h"
#include "Engine/Application.h"
#include "Renderer/DX12Renderer.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace ChickenEngine
{
	int ImguiManager::IMcount = 0;
	ImguiManager& ImguiManager::GetInstance()
	{
		static ImguiManager im;
		return im;
	}

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

		LOG_INFO("Check version");
		IMGUI_CHECKVERSION();
		LOG_INFO("Create context");
		ImGui::CreateContext();
		LOG_INFO("Get io");
		ImGuiIO& io = ImGui::GetIO(); 
		(void)io;
		
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

		LOG_INFO("Before Imgui Init");
		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

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

	bool ImguiManager::ImguiMsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

	}

	void ImguiManager::ImguiEnd()
	{
		Application& App = Application::Get();
		const DX12Renderer& DXContext = DX12Renderer::GetInstance();
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(App.GetWindow()->GetWidth(), App.GetWindow()->GetHeight());
		ImGui::Render();
		DXContext.CommandList()->SetDescriptorHeaps(1, DXContext.SrvHeap().GetAddressOf());
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DXContext.CommandList().Get());
	}


	void ImguiManager::OnEvent(Event& event)
	{
		if (!bEnabled)
			return;
		ImGuiIO& io = ImGui::GetIO();
		event.Handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
		event.Handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
	}

}