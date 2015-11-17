// DLL_Draw.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"


#define EXPORT __declspec(dllexport)

HHOOK hHook = NULL;
HINSTANCE hInstLib;
HWND _hWnd;

LRESULT CALLBACK KeyBoardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(hHook, nCode, wParam, lParam);

	KBDLLHOOKSTRUCT *str = (KBDLLHOOKSTRUCT*)lParam;
	if (str->vkCode == VK_LSHIFT || str->vkCode == VK_RSHIFT)
		if (wParam == WM_KEYDOWN)
			SendMessage(_hWnd, WM_KEYDOWN, VK_CONTROL, 0);
		else
			SendMessage(_hWnd, WM_KEYUP, VK_CONTROL, 0);

	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

EXPORT void InstallHook_Draw(HWND hWnd)
{
	if (hHook != NULL)
		return;
	_hWnd = hWnd;
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyBoardProc, hInstLib, 0);
}

EXPORT void UninstallHook_Draw()
{
	if (hHook == NULL)
		return;

	UnhookWindowsHookEx(hHook);
	hHook = NULL;
}