#pragma once
#include <cstdint>
#include <cstdio>
#include <windows.h>
#include "application.h"

LRESULT CALLBACK WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

namespace static_config
{
	constexpr wchar_t ClassName[] = L"DarkScreenClassNew";
	inline uint8_t default_opacity = 0x50;
	inline WNDCLASSEX create_window_class()
	{
		WNDCLASSEX wc;
		ZeroMemory( &wc, sizeof( WNDCLASSEX ) );

		wc.cbSize = sizeof( WNDCLASSEX );
		wc.style = CS_VREDRAW | CS_HREDRAW;
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = application_meta::application_base;
		wc.hCursor = LoadCursor( NULL, IDC_ARROW );
		wc.lpszClassName = static_config::ClassName;
		RegisterClassEx( &wc );
		return wc;
	}
}

class window_overlay
{
public:
	// cover desktop
	// TODO: implement multi monitor dimming and individual monitor dimming.
	window_overlay()
		: covered_window_handle(reinterpret_cast<HWND>(-1)),
		  window_class(static_config::create_window_class()),
		  desktop_overlay(true),
	      opacity(static_config::default_opacity)
	{
		const auto width = GetSystemMetrics( SM_CXSCREEN );
		const auto height = GetSystemMetrics( SM_CYSCREEN );

		const RECT wr = { 0, 0, width, height };
	    window_handle = CreateWindowExW(NULL, static_config::ClassName, L"DarkOverlay", WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_POPUP, 0, 0, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, application_meta::application_base, this);
	}

	window_overlay(HWND covered_window_handle)
		: covered_window_handle(covered_window_handle),
		  window_class(static_config::create_window_class()),
		  desktop_overlay(false),
		  opacity(static_config::default_opacity)
	{
		// Get position and size of the covered window
		RECT rect;
		GetWindowRect(covered_window_handle, &rect);

		// Calculate width and height
		const int width = rect.right - rect.left;
		const int height = rect.bottom - rect.top;
		
	    CreateWindowExW(NULL, static_config::ClassName, L"DarkOverlay", WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_POPUP, rect.left, rect.top, width, height, NULL, NULL, application_meta::application_base, this);
	}

	void set_window_style_settings() const
	{
		const LONG previous_window_style = GetWindowLongA(window_handle, GWL_EXSTYLE);
	    SetWindowLongA(window_handle, GWL_EXSTYLE, previous_window_style | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED);

		constexpr unsigned opacity_flag = LWA_ALPHA;
		constexpr unsigned color_key = 0x000000;

		SetLayeredWindowAttributes( window_handle, color_key, opacity, opacity_flag );
		ShowWindow( window_handle, SW_SHOW );
	}

	LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) 
	{
	    if (message == WM_PAINT)
	    {
	        PAINTSTRUCT ps;
	        const HDC hdc = BeginPaint( window_handle, &ps );

	        // All painting occurs here, between BeginPaint and EndPaint.
	        const HBRUSH hBrush = CreateSolidBrush( RGB( 0, 0, 0 ) );
	        FillRect( hdc, &ps.rcPaint, hBrush );

	        EndPaint( window_handle, &ps );
	    }
		if(message == WM_KEYDOWN)
		{
			const bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
	        const bool shiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

			if (ctrlPressed && shiftPressed)
            {
				if(wParam == VK_OEM_PLUS)
					opacity = min(opacity + 1, 255);
				else if(wParam == VK_OEM_MINUS)
					opacity = max(opacity - 1, 0);
            }
		}

	    return DefWindowProc( window_handle, message, wParam, lParam );
	}

	void Update() const
	{
		if(desktop_overlay)
		{
			SetWindowPos(window_handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE );
		}
		else
		{
			// Get position and size of the covered window
			RECT rect;
			GetWindowRect(covered_window_handle, &rect);

			// Calculate width and height
			const int width = rect.right - rect.left;
			const int height = rect.bottom - rect.top;

			// Set position and size of the overlay window
			SetWindowPos(window_handle, covered_window_handle, rect.left, rect.top, width, height, SWP_SHOWWINDOW | SWP_ASYNCWINDOWPOS);
		}

        set_window_style_settings();
	}

	HWND window_handle{};
private:
	HWND covered_window_handle;

	WNDCLASSEX window_class;
	bool desktop_overlay;

    uint8_t opacity;
};