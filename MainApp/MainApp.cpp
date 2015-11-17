// MainApp.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MainApp.h"
#include <windowsx.h>
#include <shellapi.h>
#include "SupportingFuncs.h"
#include "../IDs.h"

#define MAX_LOADSTRING 200
#define WM_TRAYICON (WM_USER + 1)
#define TRAY_ICON_ID 12345

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND hMainDlg;
WNDPROC OldEditProc;
NOTIFYICONDATA notifyIconData;

INT DIRECTION[4] = { VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN }; //default direction control values
INT BUTTON[5] = { 'A', 'D', 'S', 'W', 'X' };
INT SPEED = 10;
HWND hWndDraw = NULL;
BOOL paint = FALSE, draw = FALSE, active = FALSE;
INT px1, px2, py1, py2;

// Forward declarations of functions included in this code module:
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	NewEditProc(HWND, UINT, WPARAM, LPARAM);
ATOM				MyRegisterClass_Draw(HINSTANCE hInstance);
LRESULT CALLBACK	WndProc_Draw(HWND, UINT, WPARAM, LPARAM);

void OnInitDlg(HWND);

void SetDataToArray(INT IDEDIT, INT value);
void InstallCtrlMouseHook(HWND hWndApp);
void InstallHook_Draw(HWND hWnd);
void UninstallCtrlMouseHook();
void UninstallHook_Draw();
void UpdateData(INT dir[], INT but[], INT spd);
void Minimize(HWND);
void Restore(HWND);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MAINAPP, szWindowClass, MAX_LOADSTRING);

	MyRegisterClass_Draw(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAINAPP));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (hMainDlg == NULL || !IsDialogMessage(hMainDlg, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass_Draw(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc_Draw;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINAPP));
	wcex.hCursor = LoadCursor(NULL, IDC_CROSS);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = L"WindowDraw";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
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

   hMainDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAINDLG), NULL, DlgProc);

   if (!hMainDlg)
   {
      return FALSE;
   }

   ShowWindow(hMainDlg, SW_SHOW);

   memset(&notifyIconData, 0, sizeof(NOTIFYICONDATA));
   notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
   notifyIconData.hWnd = hMainDlg;
   notifyIconData.uID = TRAY_ICON_ID;
   notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_INFO;
   notifyIconData.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINAPP));
   notifyIconData.uCallbackMessage = WM_TRAYICON;
   wsprintf(notifyIconData.szInfo, L"Press Ctrl+Shift+M to activate on-key mouse.\nHold Shift + F2 to draw on screen.");
   wcscpy(notifyIconData.szInfoTitle, L"How to use?");

   Shell_NotifyIcon(NIM_DELETE, &notifyIconData);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			//DialogBox(hInst, MAKEINTRESOURCE(IDD_MAINDLG), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for Dialog
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == RegisterWindowMessage(L"TaskbarCreated") && !IsWindowVisible(hDlg))
	{
		Minimize(hDlg);
		return (INT_PTR)TRUE;
	}

	TCHAR buffer[MAX_LOADSTRING];
	INT tmp;

	switch (message)
	{
	case WM_INITDIALOG:
		OnInitDlg(hDlg);
		InstallCtrlMouseHook(hDlg);
		InstallHook_Draw(hDlg);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			DestroyWindow(hDlg);
			return (INT_PTR)TRUE;
		case IDC_BUTTONAPPLY:
			UpdateData(DIRECTION, BUTTON, SPEED);
			EnableWindow(GetDlgItem(hMainDlg, IDC_BUTTONAPPLY), FALSE);
			return (INT_PTR)TRUE;
		case IDC_EDITSPEED:
			if (HIWORD(wParam) != EN_CHANGE)
				break;
			Edit_GetText(GetDlgItem(hDlg, IDC_EDITSPEED), buffer, MAX_LOADSTRING);
			tmp = _wtoi(buffer);
			if (tmp > 0 && tmp <= 10000000)
				EnableWindow(GetDlgItem(hMainDlg, IDC_BUTTONAPPLY), TRUE), SPEED = tmp;
			else
				EnableWindow(GetDlgItem(hMainDlg, IDC_BUTTONAPPLY), FALSE);
			return (INT_PTR)TRUE;
		}
		break;
	case WM_KEYDOWN:
		if (wParam == VK_CONTROL && !draw && active) {
			hWndDraw = CreateWindow(L"WindowDraw", NULL, WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInst, NULL);

			draw = TRUE;

			SetWindowLong(hWndDraw, GWL_EXSTYLE, GetWindowLong(hWndDraw, GWL_EXSTYLE) | WS_EX_LAYERED);
			SetLayeredWindowAttributes(hWndDraw, 0, 255, LWA_ALPHA);

			RECT rect;
			SetWindowPos(hWndDraw, NULL, 0, 0, GetDeviceCaps(GetDC(NULL), HORZRES), GetDeviceCaps(GetDC(NULL), VERTRES), SWP_FRAMECHANGED);
			GetWindowRect(hWndDraw, &rect);
			HRGN hrgn;
			hrgn = CreateRectRgn(0, 32, GetDeviceCaps(GetDC(NULL), HORZRES), GetDeviceCaps(GetDC(NULL), VERTRES));
			SetWindowRgn(hWndDraw, hrgn, true);

			ShowWindow(hWndDraw, SW_MAXIMIZE);
		}
		break;
	case WM_KEYUP:
		if (wParam == VK_CONTROL){
			ShowWindow(hWndDraw, SW_HIDE);
			draw = FALSE;
			hWndDraw = NULL;
		}
		break;
	case WM_CLOSE:
		Minimize(hDlg);
		active = TRUE;
		return (INT_PTR)TRUE;
	case WM_TRAYICON:
		if (wParam == TRAY_ICON_ID && lParam == WM_LBUTTONUP){
			Restore(hDlg);
			active = FALSE;
		}
		return (INT_PTR)TRUE;

	case WM_DESTROY:
		UninstallCtrlMouseHook();
		UninstallHook_Draw();
		PostQuitMessage(0);
		return (INT_PTR)TRUE;
	}
	return (INT_PTR)FALSE;
}

LRESULT CALLBACK NewEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		TCHAR buf[100];
		GetKeyName(wParam, buf);
		if (wcslen(buf) > 0)
		{
			Edit_SetText(hWnd, buf);
			SetDataToArray(GetDlgCtrlID(hWnd), wParam);
			EnableWindow(GetDlgItem(hMainDlg, IDC_BUTTONAPPLY), TRUE);
		}
		return 0;

	default:
		if (OldEditProc != NULL)
			return CallWindowProc(OldEditProc, hWnd, message, wParam, lParam);
		else
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

LRESULT CALLBACK WndProc_Draw(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc, hdcSource, hdcMemory;
	HBITMAP hBitmap, hBitmapOld;
	HWND hWndDraw;

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		hdcSource = GetDC(NULL);
		hdcMemory = CreateCompatibleDC(hdcSource);

		RECT rect; int w, h;
		if (GetWindowRect(hWnd, &rect))
		{
			w = rect.right - rect.left;
			h = rect.bottom - rect.top;
		}

		hBitmap = CreateCompatibleBitmap(hdcSource, w, h);
		hBitmapOld = (HBITMAP)SelectObject(hdcMemory, hBitmap);

		BitBlt(hdc, 0, 0, w, h, hdcSource, 0, 23, SRCCOPY);
		hBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmapOld);

		DeleteDC(hdcSource);
		DeleteDC(hdcMemory);

		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		paint = TRUE;

		px1 = px2 = GET_X_LPARAM(lParam);
		py1 = py2 = GET_Y_LPARAM(lParam);

		break;
	case WM_MOUSEMOVE:
		if (!paint)
			break;
		hdc = GetDC(hWnd);
		px2 = GET_X_LPARAM(lParam);
		py2 = GET_Y_LPARAM(lParam);

		HPEN hPen;
		hPen = CreatePen(PS_SOLID, 5, RGB(255, 0, 0));
		SelectObject(hdc, hPen);

		MoveToEx(hdc, px1, py1, NULL);
		LineTo(hdc, px2, py2);

		px1 = px2;
		py1 = py2;

		ReleaseDC(hWnd, hdc);
		break;
	case WM_LBUTTONUP:
		paint = FALSE;
		break;
	case WM_DESTROY:
		typedef VOID(*MYPROC)();

		HINSTANCE hInstLib;
		MYPROC ProcAddr;

		hInstLib = LoadLibrary(L"DLL.dll");
		if (hInstLib != NULL) {
			ProcAddr = (MYPROC)GetProcAddress(hInstLib, "RemoveHok");
			if (ProcAddr != NULL)
				ProcAddr();
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


void Minimize(HWND hDlg)
{
	Shell_NotifyIcon(NIM_ADD, &notifyIconData);
	ShowWindow(hDlg, SW_HIDE);
}

void Restore(HWND hDlg)
{
	Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
	ShowWindow(hDlg, SW_SHOW);
}

void SetDataToArray(INT IDEDIT, INT value)
{
	switch (IDEDIT)
	{
	case IDC_EDITLEFT:
		DIRECTION[LEFT] = value;
		break;
	case IDC_EDITUP:
		DIRECTION[UP] = value;
		break;
	case IDC_EDITRIGHT:
		DIRECTION[RIGHT] = value;
		break;
	case IDC_EDITDOWN:
		DIRECTION[DOWN] = value;
		break;
	case IDC_EDITLBUTTON:
		BUTTON[LBUTTON] = value;
		break;
	case IDC_EDITRBUTTON:
		BUTTON[RBUTTON] = value;
		break;
	case IDC_EDITMBUTTON:
		BUTTON[MBUTTON] = value;
		break;
	case IDC_EDITWUP:
		BUTTON[WHEELUP] = value;
		break;
	case IDC_EDITWDOWN:
		BUTTON[WHEELDOWN] = value;
		break;
	}
}

void OnInitDlg(HWND hDlg)
{
	UpdateData(DIRECTION, BUTTON, SPEED);

	OldEditProc = (WNDPROC)GetWindowLongPtr(GetDlgItem(hDlg, IDC_EDITRIGHT), GWLP_WNDPROC);
	SetWindowLongPtr(GetDlgItem(hDlg, IDC_EDITRIGHT), GWLP_WNDPROC, (LONG_PTR)NewEditProc);
	SetWindowLongPtr(GetDlgItem(hDlg, IDC_EDITLEFT), GWLP_WNDPROC, (LONG_PTR)NewEditProc);
	SetWindowLongPtr(GetDlgItem(hDlg, IDC_EDITUP), GWLP_WNDPROC, (LONG_PTR)NewEditProc);
	SetWindowLongPtr(GetDlgItem(hDlg, IDC_EDITDOWN), GWLP_WNDPROC, (LONG_PTR)NewEditProc);
	SetWindowLongPtr(GetDlgItem(hDlg, IDC_EDITLBUTTON), GWLP_WNDPROC, (LONG_PTR)NewEditProc);
	SetWindowLongPtr(GetDlgItem(hDlg, IDC_EDITRBUTTON), GWLP_WNDPROC, (LONG_PTR)NewEditProc);
	SetWindowLongPtr(GetDlgItem(hDlg, IDC_EDITMBUTTON), GWLP_WNDPROC, (LONG_PTR)NewEditProc);
	SetWindowLongPtr(GetDlgItem(hDlg, IDC_EDITWUP), GWLP_WNDPROC, (LONG_PTR)NewEditProc);
	SetWindowLongPtr(GetDlgItem(hDlg, IDC_EDITWDOWN), GWLP_WNDPROC, (LONG_PTR)NewEditProc);

	TCHAR buffer[100];
	Edit_SetText(GetDlgItem(hDlg, IDC_EDITDOWN), GetKeyName(DIRECTION[DOWN], buffer));
	Edit_SetText(GetDlgItem(hDlg, IDC_EDITUP), GetKeyName(DIRECTION[UP], buffer));
	Edit_SetText(GetDlgItem(hDlg, IDC_EDITLEFT), GetKeyName(DIRECTION[LEFT], buffer));
	Edit_SetText(GetDlgItem(hDlg, IDC_EDITRIGHT), GetKeyName(DIRECTION[RIGHT], buffer));
	Edit_SetText(GetDlgItem(hDlg, IDC_EDITLBUTTON), GetKeyName(BUTTON[LBUTTON], buffer));
	Edit_SetText(GetDlgItem(hDlg, IDC_EDITRBUTTON), GetKeyName(BUTTON[RBUTTON], buffer));
	Edit_SetText(GetDlgItem(hDlg, IDC_EDITMBUTTON), GetKeyName(BUTTON[MBUTTON], buffer));
	Edit_SetText(GetDlgItem(hDlg, IDC_EDITWUP), GetKeyName(BUTTON[WHEELUP], buffer));
	Edit_SetText(GetDlgItem(hDlg, IDC_EDITWDOWN), GetKeyName(BUTTON[WHEELDOWN], buffer));

	_itow(SPEED, buffer, 10);
	Edit_SetText(GetDlgItem(hDlg, IDC_EDITSPEED), buffer);
}