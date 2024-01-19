#include <windows.h>
#include <iostream>
#include <vector>
#include <mutex>
#include <thread>

#include "window.hpp"
#include "application.h"
#include "window_picker.hpp"

std::vector<window_overlay> overlays;
//std::mutex overlays_mutex;

void overlay_manager()
{
	while (true) // TODO: Replace with a condition to exit appropriately
    {
        // Check for WIN+SHIFT+B
        if ((GetAsyncKeyState(VK_LWIN) & 0x8000) &&  // Left Win key is down
            (GetAsyncKeyState(VK_SHIFT) & 0x8000) && // Shift key is down
            (GetAsyncKeyState(0x42) & 0x0001))       // 'B' key is pressed
        {
            // The key combination WIN+SHIFT+B was detected
            const HWND selectedWindow = SelectWindow(application_meta::application_base); // Implement this function to select a window
            if (selectedWindow != NULL)
            {
                //std::scoped_lock lock{overlays_mutex};
                overlays.emplace_back(selectedWindow);
            }
        }

        // Add a small delay to prevent the loop from consuming too much CPU
        Sleep(0);
    }
}

int wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow )
{
    application_meta::application_base = hInstance;
    SetProcessDpiAwarenessContext( DPI_AWARENESS_CONTEXT_UNAWARE );

#ifdef _DEBUG
    AllocConsole( );

    FILE* file;
    int err;
    err = freopen_s(&file, "CONIN$", "r", stdin );
    err = freopen_s(&file, "CONOUT$", "w", stdout );
    err = freopen_s(&file, "CONOUT$", "w", stderr );
#endif

	//std::thread manager_thread(overlay_manager);
    //manager_thread.detach();

    overlays.emplace_back(FindWindowA("Notepad", nullptr));

	MSG msg;
	while (true)
	{
        bool result = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

        if (result) {
            // Process the message
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }


	    {
		    //std::scoped_lock lock{overlays_mutex};
		    for (auto& overlay : overlays)
		    {
		        overlay.Update(); 
		    }
	    }

        Sleep(0);
	}
}

