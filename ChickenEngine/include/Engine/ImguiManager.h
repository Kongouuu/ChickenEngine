#pragma once

#include "Core.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx12.h"
#include "Scene/RenderObject.h"
#include "Helper/Singleton.h"
#include <Engine/Events/MouseEvent.h>
#include <Engine/Events/KeyEvent.h>
#include <Engine/Events/ApplicationEvent.h>

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::Text(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))
#define IMGUI_LEFT_LABEL2(func, label, labelHide, ...) (ImGui::Text(label), ImGui::SameLine(), func("##" labelHide, __VA_ARGS__))

namespace ChickenEngine
{
	class CHICKEN_API ImguiManager : public Singleton<ImguiManager>
	{
	public:
		ImguiManager();
		~ImguiManager();
		void ImguiInit();
		void ImguiDestroy();
		bool ImguiMsgProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam);
		void ImguiBegin();
		void ImguiEnd();
		void ImguiRender();
		void OnEvent(Event& event);

		inline static float ViewportWidth() { return instance().mViewportWidth; }
		inline static float ViewportHeight() { return instance().mViewportHeight; }
		inline static bool ViewportSizeDirty() { bool dirty = instance().mViewportSizeDirty; instance().mViewportSizeDirty = false; return dirty; }
	private:
		void ShowDockSpace();
		void ShowViewPortPanel();
		void ShowScenePanel();
		void ShowDetailPanel();
		void ShowCameraPanel();
		void ShowLightPanel();
		void ShowSettingsPanel();
		void ShowStatsPanel();
	private:
		bool bEnabled;
		static int IMcount;

		float mViewportWidth = 0;
		float mViewportHeight = 0;
		bool mViewportSizeDirty = false;

		std::shared_ptr<RenderObject> ro = nullptr;
	};

}

