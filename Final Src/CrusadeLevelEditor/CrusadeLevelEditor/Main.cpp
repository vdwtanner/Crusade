//
// Main.cpp -
//

#include "pch.h"
#include "Game.h"
#include "resource.h"
#include "CommCtrl.h"
#include "InputProcessor.h"
#include <string>
#include <sstream>
#include "stdlib.h"

#define TOOLBAR_WIDTH 32

using namespace DirectX;

namespace
{
    std::unique_ptr<Game> g_game;
	HIMAGELIST g_hImageList = nullptr;
	HWND directx_hwnd;
	HWND hwnd;
	HWND listbox_hwnd;
	HWND toolbar_hwnd;
	InputProcessor *inputProcessor;
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DirectXProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND CreateVericalToolbar(HWND, HINSTANCE, int, int, int, int);

HWND CreateVerticalToolbar(HWND parent, HINSTANCE instance, int x, int y, int width, int height){

	const int numButtons = 3;

	TBBUTTON buttons[numButtons] = {
			{ STD_HELP, IDT_SELECT, TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON, 0 },
			{ STD_PRINT, IDT_DRAW, TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON, 0 },
			{ STD_DELETE, IDT_ERASE, TBSTATE_ENABLED | TBSTATE_WRAP, BTNS_BUTTON, 0 }
	};

	HWND hwndTool = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | WS_VISIBLE | CCS_VERT | WS_BORDER | CCS_NORESIZE, x, y, width, height, parent,
		nullptr, instance, nullptr);

	g_hImageList = ImageList_Create(24, 24, ILC_COLOR16 | ILC_MASK, numButtons, 0);

	SendMessage(hwndTool, TB_SETIMAGELIST, 0, (LPARAM)g_hImageList);

	SendMessage(hwndTool, TB_LOADIMAGES, (WPARAM)IDB_STD_LARGE_COLOR, (LPARAM)HINST_COMMCTRL);

	SendMessage(hwndTool, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(hwndTool, TB_ADDBUTTONS, numButtons, (LPARAM)buttons);

	return hwndTool;

}

// Entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr))
        return 1;

	g_game.reset(new Game());

	// Register class and create window
	{
		// Register class
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(hInstance, L"IDI_ICON");
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = MAKEINTRESOURCE(IDM_MAINMENU);
		wcex.lpszClassName = L"CrusadeLevelEditorWindowClass";
		wcex.hIconSm = LoadIcon(wcex.hInstance, L"IDI_ICON");
		if (!RegisterClassEx(&wcex))
			return 1;

		// Create window
		size_t w, h;
		g_game->GetDefaultSize(w, h);

		RECT rc;
		rc.top = 0;
		rc.left = 0;
		rc.right = static_cast<LONG>(w);
		rc.bottom = static_cast<LONG>(h);
		hwnd = CreateWindow(L"CrusadeLevelEditorWindowClass", L"CrusadeLevelEditor", WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
			nullptr);
		if (!hwnd)
			return 1;

		GetClientRect(hwnd, &rc);

		// Register class
		WNDCLASSEX dx_wcex;
		dx_wcex.cbSize = sizeof(WNDCLASSEX);
		dx_wcex.style = CS_HREDRAW | CS_VREDRAW;
		dx_wcex.lpfnWndProc = DirectXProc;
		dx_wcex.cbClsExtra = 0;
		dx_wcex.cbWndExtra = 0;
		dx_wcex.hInstance = hInstance;
		dx_wcex.hIcon = LoadIcon(hInstance, L"IDI_ICON");
		dx_wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		dx_wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		dx_wcex.lpszMenuName = 0;
		dx_wcex.lpszClassName = L"DirectxViewport";
		dx_wcex.hIconSm = LoadIcon(dx_wcex.hInstance, L"IDI_ICON");
		if (!RegisterClassEx(&dx_wcex))
			return 1;

		float dx_x = ((float)(rc.right - rc.left) / 6.f) + TOOLBAR_WIDTH;
		float dx_y = 0;
		float dx_width = (float)(rc.right - rc.left)*(5.f / 6.f) - TOOLBAR_WIDTH;
		float dx_height = (rc.bottom - rc.top);

		directx_hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"DirectxViewport", nullptr,
			WS_CHILD | WS_VISIBLE, dx_x, dx_y, dx_width, dx_height, hwnd, nullptr, hInstance,
			nullptr);
		if (!directx_hwnd)
			return 1;

		float lb_x = TOOLBAR_WIDTH;
		float lb_y = 0;
		float lb_width = (rc.right - rc.left) / 6.f;
		float lb_height = (rc.bottom - rc.top);

		listbox_hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"LISTBOX", nullptr,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_DISABLENOSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT,
			lb_x, lb_y, lb_width, lb_height, hwnd, nullptr, hInstance, nullptr);
		if (!listbox_hwnd)
			return 1;

		//SendMessage(listbox_hwnd, LB_ADDSTRING, 0, (LPARAM)L"TestString");

		toolbar_hwnd = CreateVerticalToolbar(hwnd, hInstance, 0, 0, TOOLBAR_WIDTH, (rc.bottom - rc.top));

        ShowWindow(hwnd, nCmdShow);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(g_game.get()) );

        g_game->Initialize( directx_hwnd );
		inputProcessor = new InputProcessor(g_game.get());
		inputProcessor->giveHandles(hwnd, directx_hwnd, listbox_hwnd, toolbar_hwnd);

    }

    // Main message loop
    MSG msg = { 0 };
	int x = 0;
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            g_game->Tick();
        }
    }

    g_game.reset();

	delete inputProcessor;
	CoUninitialize();


    return (int) msg.wParam;
}

LRESULT CALLBACK DirectXProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){

	return inputProcessor->MainWndProc(hWnd, message, wParam, lParam);

}


// Windows procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    static bool s_in_sizemove = false;
    static bool s_in_suspend = false;
    static bool s_minimized = false;

    auto game = reinterpret_cast<Game*>( GetWindowLongPtr(hWnd, GWLP_USERDATA) );

    switch (message)
	{
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            if (!s_minimized)
            {
                s_minimized = true;
                if (!s_in_suspend && game)
                    game->OnSuspending();
                s_in_suspend = true;
            }
        }
        else if (s_minimized)
        {
            s_minimized = false;
            if (s_in_suspend && game)
                game->OnResuming();
            s_in_suspend = false;
        }
		else if (!s_in_sizemove && game){

			RECT lb_rc;
			GetWindowRect(listbox_hwnd, &lb_rc);
			float lb_width = (lb_rc.right - lb_rc.left);

			RECT rc;
			GetClientRect(hwnd, &rc);

			float x = rc.left;
			float y = 0;
			float width = ((float)rc.right - (float)rc.left) - lb_width - TOOLBAR_WIDTH - 1;
			float height = ((float)rc.bottom - (float)rc.top);


			SetWindowPos(directx_hwnd, 0, x, y, width, height, SWP_NOMOVE);
			SetWindowPos(toolbar_hwnd, 0, 0, 0, TOOLBAR_WIDTH, height, SWP_NOMOVE);
			SetWindowPos(listbox_hwnd, 0, TOOLBAR_WIDTH, 0, lb_width, height, SWP_NOMOVE);

			game->OnWindowSizeChanged();

		}
        break;

    case WM_ENTERSIZEMOVE:
        s_in_sizemove = true;
        break;

    case WM_EXITSIZEMOVE:
        s_in_sizemove = false;
		if (game){

			RECT lb_rc;
			GetWindowRect(listbox_hwnd, &lb_rc);
			float lb_width = (lb_rc.right - lb_rc.left);

			RECT rc;
			GetClientRect(hwnd, &rc);

			float x = rc.left;
			float y = 0;
			float width = ((float)rc.right - (float)rc.left) - lb_width - TOOLBAR_WIDTH - 1;
			float height = ((float)rc.bottom - (float)rc.top);


			SetWindowPos(directx_hwnd, 0, x, y, width, height, SWP_NOMOVE);
			SetWindowPos(toolbar_hwnd, 0, 0, 0, TOOLBAR_WIDTH, height, SWP_NOMOVE);
			SetWindowPos(listbox_hwnd, 0, TOOLBAR_WIDTH, 0, lb_width, height, SWP_NOMOVE);

			game->OnWindowSizeChanged();
		}
        break;

    case WM_GETMINMAXINFO:
        {
            auto info = reinterpret_cast<MINMAXINFO*>(lParam);
            info->ptMinTrackSize.x = 320;
            info->ptMinTrackSize.y = 200;
        }
        break;

    case WM_ACTIVATEAPP:
        if (game)
        {
            if (wParam)
            {
                game->OnActivated();
            }
            else
            {
                game->OnDeactivated();
            }
        }
        break;

    case WM_POWERBROADCAST:
        switch (wParam)
        {
        case PBT_APMQUERYSUSPEND:
            if (!s_in_suspend && game)
                game->OnSuspending();
            s_in_suspend = true;
            return true;

        case PBT_APMRESUMESUSPEND:
            if (!s_minimized)
            {
                if (s_in_suspend && game)
                    game->OnResuming();
                s_in_suspend = false;
            }
            return true;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return inputProcessor->MainWndProc(hWnd, message, wParam, lParam);
}