#include "window.hpp"

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    window_overlay* pThis = nullptr;

    if (message == WM_NCCREATE)
    {
        // Retrieve the instance pointer passed during window creation
        const auto pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = static_cast<window_overlay*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        pThis->window_handle = hWnd;
        pThis->set_window_style_settings();
    }
    else
    {
        pThis = reinterpret_cast<window_overlay*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pThis)
    {
        // Delegate to a member function of the instance
        return pThis->HandleMessage(message, wParam, lParam);
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}