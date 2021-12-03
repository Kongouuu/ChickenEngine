#pragma once

#include "Core.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx12.h"
#include <Engine/Events/MouseEvent.h>
#include <Engine/Events/KeyEvent.h>
#include <Engine/Events/ApplicationEvent.h>

namespace ChickenEngine
{
	class CHICKEN_API ImguiManager
	{
	public:
		ImguiManager();
		~ImguiManager();
		void ImguiInit();
		void ImguiDestroy();
		bool ImguiMsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void ImguiBegin();
		void ImguiEnd();
		void ImguiRender();
		void OnEvent(Event& event);

		static ImguiManager& GetInstance();

	protected:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);
	private:
		bool bEnabled;
		static int IMcount;
	};

}

