#include "pch.h"
#include "Engine/Window.h"
#include "Engine/Core.h"
#include <tchar.h>
#include <DirectXColors.h>
#include <windowsx.h>

namespace ChickenEngine
{
	LRESULT CALLBACK
		MainWndProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
	{
		return Window::GetWindow()->MsgProc(hwnd, msg, wParam, lParam);
	}

	Window* Window::_mWindow = nullptr;

	Window::Window()
	{
		_mWindow = this;
	}

	Window::~Window()
	{
	}


	void Window::Init(std::string title, unsigned int width, unsigned int height)
	{
		/***************** Init Window *********************/
		mTitle = title;
		mWidth = width;
		mHeight = height;

		if (!InitWindow())
		{
			LOG_ERROR("Init window fail");
		}
	}

	void Window::Shutdown()
	{
		WindowCloseEvent e;
		mEventCallback(e);
		::DestroyWindow(mhMainWnd);

		PostQuitMessage(0);
	}

	// Events
	LRESULT Window::MsgProc(HWND hwnd, uint32_t msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImguiManager::instance().ImguiMsgProc(hwnd, msg, wParam, lParam))
		{
			return true;
		}
		
		switch (msg)
		{
			// WM_ACTIVATE is sent when the window is activated or deactivated.  
			// We pause the game when the window is deactivated and unpause it 
			// when it becomes active.  
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
			{

			}
			else
			{

			}
			return 0;

			// WM_SIZE is sent when the user resizes the window.  
		case WM_SIZE:
		{
			// Save the new client area dimensions.
			mWidth = LOWORD(lParam);
			mHeight = HIWORD(lParam);
			WindowResizeEvent e(mWidth, mHeight);
			mEventCallback(e);
			return 0;
		}
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		case WM_ENTERSIZEMOVE:

			return 0;
			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
		case WM_EXITSIZEMOVE:

			return 0;

			// WM_DESTROY is sent when the window is being destroyed.
		case WM_DESTROY:
			Shutdown();
			return 0;

			// The WM_MENUCHAR message is sent when a menu is active and the user presses 
			// a key that does not correspond to any mnemonic or accelerator key. 
		case WM_MENUCHAR:
			// Don't beep when we alt-enter.
			return MAKELRESULT(0, MNC_CLOSE);

			// Catch this message so to prevent the window from becoming too small.
		case WM_GETMINMAXINFO:
			return 0;

		case WM_LBUTTONDOWN:
		{
			MouseButtonPressedEvent e(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0);
			mEventCallback(e);
			return 0;
		}
		case WM_MBUTTONDOWN:
		{
			MouseButtonPressedEvent e(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 2);
			mEventCallback(e);
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			MouseButtonPressedEvent e(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 1);
			mEventCallback(e);
			return 0;
		}
			return 0;
		case WM_LBUTTONUP:
		{
			MouseButtonReleasedEvent e(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0);
			mEventCallback(e);
			return 0;
		}
		case WM_MBUTTONUP:
		{
			MouseButtonReleasedEvent e(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 2);
			mEventCallback(e);
			return 0;
		}
		case WM_RBUTTONUP:
		{
			MouseButtonReleasedEvent e(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 1);
			mEventCallback(e);
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			MouseMovedEvent e(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			mEventCallback(e);
			return 0;
		}
		case WM_KEYDOWN:
		{
			if (wParam == VK_ESCAPE)
			{
				Shutdown();
			}
			KeyPressedEvent e(wParam, 1);
			mEventCallback(e);
			return 0;
		}
		case WM_KEYUP:
		{
			KeyReleasedEvent e(wParam);
			mEventCallback(e);
			return 0;
		}
			
		case WM_CHAR:
		{
			KeyTypedEvent e(wParam);
			mEventCallback(e);
			return 0;
		}

		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}


#pragma region Init_Functions
	bool Window::InitWindow()
	{
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = MainWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetInstanceModule(NULL);
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = L"MainWnd";

		if (!RegisterClass(&wc))
		{
			MessageBox(0, L"RegisterClass Failed.", 0, 0);
			return false;
		}

		// Compute window rectangle dimensions based on requested client area dimensions.
		RECT R = { 0, 0, mWidth, mHeight };
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		int tmpWidth = R.right - R.left;
		int tmpHeight = R.bottom - R.top;

		std::wstring wTitle = std::wstring(mTitle.begin(), mTitle.end());
		mhMainWnd = CreateWindow(L"MainWnd", wTitle.c_str(),
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, tmpWidth, tmpHeight, 0, 0, wc.hInstance, 0);

		if (!mhMainWnd)
		{
			LOG_TRACE("Create window fail");
			MessageBox(0, L"CreateWindow Failed.", 0, 0);
			return false;
		}

		LOG_TRACE("Show Window");

		ShowWindow(mhMainWnd, SW_SHOW);

		LOG_TRACE("Update Window");
		UpdateWindow(mhMainWnd);
		LOG_TRACE("after show and update.");
		return true;
	}
}


