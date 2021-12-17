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

	private:
		bool bEnabled;
		static int IMcount;
	};

}

