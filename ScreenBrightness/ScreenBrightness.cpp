#include <windows.h>
#include <iostream>

std::wstring get_computer_unique_classname( )
{
    TCHAR ComputerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD dwNameLength = MAX_COMPUTERNAME_LENGTH + 1;
    GetComputerNameW( ComputerName, &dwNameLength );

    return std::wstring( ComputerName, dwNameLength );
}

LRESULT CALLBACK WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    switch (message)
    {
        case WM_PAINT:
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint( hWnd, &ps );

            // All painting occurs here, between BeginPaint and EndPaint.
            const auto hBrush = CreateSolidBrush( RGB( 0, 0, 0 ) );
            FillRect( hdc, &ps.rcPaint, hBrush );

            EndPaint( hWnd, &ps );
    }

    return DefWindowProc( hWnd, message, wParam, lParam );
}

int wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow )
{
    SetProcessDpiAwarenessContext( DPI_AWARENESS_CONTEXT_UNAWARE );
    
    AllocConsole( );

    FILE* file;
    freopen_s(&file, "conin$", "r", stdin );
    freopen_s(&file, "conout$", "w", stdout );
    freopen_s(&file, "conout$", "w", stderr );

    HWND hwnd; WNDCLASSEX wc;
    ZeroMemory( &wc, sizeof( WNDCLASSEX ) );

    wc.cbSize = sizeof( WNDCLASSEX );
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.lpszClassName = get_computer_unique_classname().c_str();

    RegisterClassEx( &wc );

    const auto width = GetSystemMetrics( SM_CXSCREEN );
    const auto height = GetSystemMetrics( SM_CYSCREEN );

    RECT wr = { 0, 0, width, height };
   
    hwnd = CreateWindowExW(NULL, get_computer_unique_classname( ).c_str( ), L"", WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_POPUP, 0, 0, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, hInstance, NULL);
    SetWindowLongA(hwnd, GWL_EXSTYLE, GetWindowLongA(hwnd, GWL_EXSTYLE) | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED);
    
    const UINT opacity_flag = LWA_ALPHA;
    const UINT color_key = 0x000000;
    const UINT opacity = 0x50;

    SetLayeredWindowAttributes( hwnd, color_key, opacity, opacity_flag );
    ShowWindow( hwnd, SW_SHOW );

    std::printf("Brightness set. Use +/- to lower");

    MSG msg = { };
    while (GetMessage( &msg, NULL, 0, 0 ) > 0 && !GetAsyncKeyState( VK_HOME ) & 0x1)
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );

        // make sure our window stays on top
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE );

        static unsigned new_opacity = 0x50;
        if (GetAsyncKeyState( VK_OEM_PLUS ))
            new_opacity += 1;
        
        if(GetAsyncKeyState( VK_OEM_MINUS ))
            new_opacity -= 1;

        SetLayeredWindowAttributes( hwnd, color_key, new_opacity, opacity_flag ); Sleep( 100 ); // let our thread get rescheuled we're not that important :/
    }
}

