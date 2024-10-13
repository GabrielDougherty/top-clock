// Clock.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Clock.h"

#include <windows.h>
#include <cstdio>
#include <string>
#include <wingdi.h>

#define TIMER1 1001
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hClockText;
int cachedMinute{ -1 };

const int WIN_WIDTH = 134;
const int WIN_HEIGHT = 74;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLOCK, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLOCK));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLOCK));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CLOCK);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, WIN_WIDTH, WIN_HEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   SetMenu(hWnd, NULL);
   SetTimer(hWnd,             // handle to main window 
       TIMER1,            // timer identifier 
       1000,                 // 1-second interval 
       (TIMERPROC)NULL);     // no timer callback 

   SetWindowLong(hWnd, GWL_STYLE, 0); //remove all window styles, check MSDN for details

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void DrawClock(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(hWnd, &ps);
    // TODO: Add any drawing code that uses hdc here...
    HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
    HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

    // Calculate the dimensions of the 4 equal rectangles.
    RECT rcWindow;
    GetClientRect(hWnd, &rcWindow);

    RECT rc1;
    rc1 = rcWindow;

    // Optionally, deflate each of the rectangles by an arbitrary amount so that
    // they don't butt up right next to each other and we can distinguish them.
    InflateRect(&rc1, -5, -5);
    // Draw (differently-colored) borders around these rectangles.
    SetDCPenColor(hDC, RGB(255, 0, 0));    // red
    Rectangle(hDC, rc1.left, rc1.top, rc1.right, rc1.bottom);

    // Draw the text into the center of each of the rectangles.
    SetBkMode(hDC, TRANSPARENT);
    SetBkColor(hDC, RGB(0, 0, 0));   // black
    // TODO: Optionally, set a nicer font than the default.

    // Clean up after ourselves.
    SelectObject(hDC, hpenOld);
    SelectObject(hDC, hbrushOld);
    SetBkColor(hDC, RGB(0, 0, 0));
    HBRUSH brush = CreateSolidBrush(RGB(50, 151, 151));

    FillRect(hDC, &rc1, brush);

    SYSTEMTIME lt;
    GetLocalTime(&lt);
    DeleteObject(brush);
    char cstr2[100];
#ifndef NDEBUG
    sprintf_s(cstr2, " %02d:%02d:%02d %s\n", lt.wHour > 12 ? lt.wHour - 12 : lt.wHour == 0 ? 12 : lt.wHour, lt.wMinute, lt.wMilliseconds, lt.wHour >= 12 ? "PM" : "AM");
#else
    sprintf_s(cstr2, " %02d:%02d %s\n", lt.wHour > 12 ? lt.wHour - 12 : lt.wHour == 0 ? 12 : lt.wHour, lt.wMinute, lt.wHour >= 12 ? "PM" : "AM");
#endif
    auto s = std::string(cstr2);
    std::wstring stemp = std::wstring(s.begin(), s.end());
    LPCWSTR sw = stemp.c_str();
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

#ifndef NDEBUG
    auto tmp = std::string() + std::to_string(lt.wMinute) + " " + std::to_string(cachedMinute);

    std::wstring temp2 = std::wstring(tmp.begin(), tmp.end());
    OutputDebugString(temp2.c_str());
#endif

    DrawText(hDC, sw, -1, &rc1, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
    cachedMinute = lt.wMinute;

    EndPaint(hWnd, &ps);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_TIMER:
    {
        SYSTEMTIME lt;
        GetLocalTime(&lt);

        RECT rcWindow;
        GetClientRect(hWnd, &rcWindow);
        if (static_cast<int>(lt.wMinute) == cachedMinute)
        {
            break;
        }
        cachedMinute = lt.wMinute;
        RECT rc1;
        rc1 = rcWindow;

        // Optionally, deflate each of the rectangles by an arbitrary amount so that
        // they don't butt up right next to each other and we can distinguish them.
        InflateRect(&rc1, -5, -5);
        InvalidateRect(hWnd, &rc1, false);
        break;
    }
    case WM_PAINT:
        DrawClock(hWnd);
        break;
    case WM_NCHITTEST:
    {
        LRESULT hit = DefWindowProc(hWnd, message, wParam, lParam);
        if (hit == HTCLIENT) hit = HTCAPTION;
        return hit;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
