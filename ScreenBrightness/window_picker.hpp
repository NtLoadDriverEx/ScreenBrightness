#pragma once
#include <windows.h>
#include <wingdi.h>

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