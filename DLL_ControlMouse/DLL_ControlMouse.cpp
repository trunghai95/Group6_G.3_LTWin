// DLL_ControlMouse.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#define EXPORT  __declspec(dllexport)
#define IMPORT __declspec(dllimport)
HHOOK hHook = NULL;
HWND hWnd = NULL;
INT Direction[4] = { VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN }; //default values
extern HINSTANCE hInstLib;

void MoveMouse(HWND hWnd, WPARAM wParam);

LRESULT CALLBACK KBControlMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(hHook, nCode, wParam, lParam);
	MoveMouse(hWnd, wParam);
	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

EXPORT void InstallCtrlMouseHook(HWND hWndApp)
{
	hWnd = hWndApp;
	if (hHook != NULL) return;
	hHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KBControlMouseProc, hInstLib, 0);
}

EXPORT void UninstallCtrlMouseHook()
{
	if (hHook == NULL) return;
	UnhookWindowsHookEx(hHook);
	hHook = NULL;
}

EXPORT void UpdateDirection(INT dir[])
{
	for (int i = 0; i < 4; i++)
		Direction[i] = dir[i];
}

void MoveMouse(HWND hWnd, WPARAM wParam)
{
	int repeat = 10;
	POINT pt;

	GetCursorPos(&pt);

	ScreenToClient(hWnd, &pt);

	if (wParam == Direction[0]) // Left
		pt.x -= repeat;
	else if (wParam == Direction[1]) // Up
		pt.y -= repeat;
	else if (wParam == Direction[2]) // Right
		pt.x += repeat;
	else if (wParam == Direction[3]) // Down
		pt.y += repeat;

	ClientToScreen(hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}
