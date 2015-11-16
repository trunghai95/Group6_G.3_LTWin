// MainApp.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MainApp.h"
#include <windowsx.h>
#include "SupportingFuncs.h"
#include "../IDs.h"

#define MAX_LOADSTRING 200

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND hMainDlg;
WNDPROC OldEditProc;
INT DIRECTION[4] = { VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN }; //default direction control values
INT BUTTON[5] = { 'A', 'D', 'S', 'W', 'X' };
// Forward declarations of functions included in this code module:
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	NewEditProc(HWND, UINT, WPARAM, LPARAM);

void OnInitDlg(HWND);

void SetDirection(INT IDEDIT, INT value);
void InstallCtrlMouseHook(HWND hWndApp);
void UninstallCtrlMouseHook();
void UpdateDirection(INT dir[]);


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

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAINAPP));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
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
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		OnInitDlg(hDlg);
		InstallCtrlMouseHook(hDlg);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			DestroyWindow(hDlg);
			return (INT_PTR)TRUE;
		case IDC_BUTTONAPPLY:
			UpdateDirection(DIRECTION);
			EnableWindow(GetDlgItem(hMainDlg, IDC_BUTTONAPPLY), FALSE);
			return (INT_PTR)TRUE;
		}
		break;

	case WM_DESTROY:
		UninstallCtrlMouseHook();
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
		Edit_SetText(hWnd, buf);
		SetDirection(GetDlgCtrlID(hWnd), wParam);
		EnableWindow(GetDlgItem(hMainDlg, IDC_BUTTONAPPLY), TRUE);
		return 0;

	default:
		if (OldEditProc != NULL)
			return CallWindowProc(OldEditProc, hWnd, message, wParam, lParam);
		else
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

void SetDirection(INT IDEDIT, INT value)
{
	switch (IDEDIT)
	{
	case IDC_EDITLEFT:
		DIRECTION[0] = value;
		break;
	case IDC_EDITUP:
		DIRECTION[1] = value;
		break;
	case IDC_EDITRIGHT:
		DIRECTION[2] = value;
		break;
	case IDC_EDITDOWN:
		DIRECTION[3] = value;
		break;
	}
}

void OnInitDlg(HWND hDlg)
{
	OldEditProc = (WNDPROC)GetWindowLongPtr(GetDlgItem(hDlg, IDC_EDITRIGHT), GWLP_WNDPROC);
	SetWindowLongPtr(GetDlgItem(hDlg, IDC_EDITRIGHT), GWLP_WNDPROC, (LONG_PTR)NewEditProc);
	SetWindowLongPtr(GetDlgItem(hDlg, IDC_EDITLEFT), GWLP_WNDPROC, (LONG_PTR)NewEditProc);
	SetWindowLongPtr(GetDlgItem(hDlg, IDC_EDITUP), GWLP_WNDPROC, (LONG_PTR)NewEditProc);
	SetWindowLongPtr(GetDlgItem(hDlg, IDC_EDITDOWN), GWLP_WNDPROC, (LONG_PTR)NewEditProc);

	TCHAR buffer[100];
	Edit_SetText(GetDlgItem(hDlg, IDC_EDITDOWN), GetKeyName(DIRECTION[DOWN], buffer));
	Edit_SetText(GetDlgItem(hDlg, IDC_EDITUP), GetKeyName(DIRECTION[UP], buffer));
	Edit_SetText(GetDlgItem(hDlg, IDC_EDITLEFT), GetKeyName(DIRECTION[LEFT], buffer));
	Edit_SetText(GetDlgItem(hDlg, IDC_EDITRIGHT), GetKeyName(DIRECTION[RIGHT], buffer));
}