#pragma once
#include <windows.h>
#include <wingdi.h>

inline LRESULT CALLBACK windowHighlightProc(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam)
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

	return DefWindowProc(window_handle, message, wParam, lParam);
}

inline HWND hHighlightWnd = NULL;
inline HWND hWndSelected = NULL;

inline void CreateHighlightWindow(HINSTANCE hInstance)
{
    const wchar_t CLASS_NAME[]  = L"Window Highlight";

    WNDCLASS wc = {};
    wc.lpfnWndProc   = windowHighlightProc; // Default window procedure
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    hHighlightWnd = CreateWindowEx(
        WS_EX_LAYERED,
        CLASS_NAME,
        L"Highlight",
        WS_POPUP,
        0, 0, 0, 0, // Position and size will be set later
        NULL,
        NULL,
        hInstance,
        NULL
    );
    DWORD flags = 0;
	SetWindowPos(hHighlightWnd, HWND_TOPMOST, 0,0,500,500, flags);

    SetLayeredWindowAttributes(hHighlightWnd, 0, RGB(255, 0, 0), LWA_COLORKEY); // Red border
}

static VOID DrawWindowBorderForTargeting(
    _In_ HWND hWnd
    )
{
    RECT rect;
    HDC hdc;

    GetWindowRect(hWnd, &rect);
    hdc = GetWindowDC(hWnd);

    if (hdc)
    {
        INT penWidth;
        INT oldDc;
        HPEN pen;
        HBRUSH brush;

        penWidth = GetSystemMetrics(SM_CXBORDER) * 3;
        oldDc = SaveDC(hdc);

        // Get an inversion effect.
        SetROP2(hdc, R2_NOT);

        pen = CreatePen(PS_INSIDEFRAME, penWidth, RGB(0x00, 0x00, 0x00));
        SelectObject(hdc, pen);

        brush = (HBRUSH)GetStockObject(NULL_BRUSH);
        SelectObject(hdc, brush);

        // Draw the rectangle.
        Rectangle(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top);

        // Cleanup.
        DeleteObject(pen);

        RestoreDC(hdc, oldDc);
        ReleaseDC(hWnd, hdc);
    }
}
inline BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    HWND* pResult = reinterpret_cast<HWND*>(lParam);
    if (hWnd == hHighlightWnd) // Skip the "WindowSelection" window
        return TRUE;

    RECT rect;
    GetWindowRect(hWnd, &rect);
    POINT pt;
    GetCursorPos(&pt);

    if (PtInRect(&rect, pt)) {
        *pResult = hWnd;
        return FALSE; // Stop enumeration as we found the window
    }

    return TRUE; // Continue enumeration
}

inline HWND SelectWindow(HINSTANCE hInstance)
{
    for(;;Sleep(10))
    {
	    static HWND hLastHover = 0;
		POINT pt;
		GetCursorPos(&pt); // Get cursor position
		HWND hHover = WindowFromPoint(pt);
	    if (hHover != hLastHover)
	    {
	        // Remove highlight from the last window
	        DrawWindowBorderForTargeting(hLastHover);

	        // Add highlight to the new window
	        wchar_t hoverTitle[256];
	        GetWindowText(hHover, hoverTitle, 256);
	        wprintf(L"%s\n", hoverTitle);
	        DrawWindowBorderForTargeting(hHover);

	        hLastHover = hHover;
	    }

        if(GetAsyncKeyState(VK_LBUTTON))
        {
	        DrawWindowBorderForTargeting(hHover);
            return hHover;
        }
    }
}