// DLL_Draw.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"


#define EXPORT __declspec(dllexport)
#define VK_D 'D'

HHOOK hHook = NULL;
HINSTANCE hInstLib;
HWND _hWnd;

LRESULT CALLBACK KeyBoardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(hHook, nCode, wParam, lParam);

	KBDLLHOOKSTRUCT* str = (KBDLLHOOKSTRUCT*)lParam;
	if (GetKeyState(VK_SHIFT) && str->vkCode == VK_F2 && wParam == WM_KEYDOWN)
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