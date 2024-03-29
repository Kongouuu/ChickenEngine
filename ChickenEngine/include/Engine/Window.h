#pragma once

#include "pch.h"
#include "Core.h"
#include "Events/Event.h"
#include "Log.h"
#include "ImguiManager.h"

#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx12.h"

namespace ChickenEngine
{
	const int NUM_FRAMES_IN_FLIGHT = 2;
	const int NUM_BACK_BUFFERS = 2;
	class CHICKEN_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;
		Window();
		virtual ~Window();

		void Init(std::string title, unsigned int width, unsigned int height);
		void Shutdown();

		virtual LRESULT CALLBACK MsgProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam);


		inline void SetEventCallBack(const EventCallbackFn& callback) {mEventCallback = callback;}
	public:
		inline HWND MainWnd() const { return mhMainWnd; }
		inline float AspectRatio() const { return mWidth / mHeight; }
		inline unsigned int GetWidth() { return mWidth; }
		inline unsigned int GetHeight() { return mHeight; }

		inline static Window* GetWindow() {return _mWindow; }
		
	protected:
		// DX12 Initialization
		bool InitWindow();
	private:
		// Common Variable
		static Window* _mWindow;
		std::string mTitle;
		unsigned int mWidth;
		unsigned int mHeight;
		HINSTANCE mhInstance = nullptr;
		HWND      mhMainWnd = nullptr; // main window handle
		EventCallbackFn mEventCallback;

							   


	};
}
