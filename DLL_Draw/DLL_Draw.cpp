// DLL_Draw.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"


#define EXPORT __declspec(dllexport)
#define VK_D 'D'

HHOOK hHookKeyBoard = NULL;
HHOOK hHookMouse = NULL;
INT DRAWING = FALSE;
HINSTANCE hInstLib;
HWND _hWnd, hWndDraw;

LRESULT CALLBACK KeyBoardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(hHookKeyBoard, nCode, wParam, lParam);
	
	if (GetActiveWindow() != hWndDraw && DRAWING && wParam == WM_KEYUP){
		SendMessage(_hWnd, WM_KEYDOWN, VK_CONTROL, 0);
		DRAWING = !DRAWING;
	}

	KBDLLHOOKSTRUCT* str = (KBDLLHOOKSTRUCT*)lParam;
	if (GetKeyState(VK_SHIFT) && str->vkCode == VK_F2 && wParam == WM_KEYDOWN){
		SendMessage(_hWnd, WM_KEYDOWN, VK_CONTROL, 0);
		DRAWING = !DRAWING;
	}
	
	return CallNextHookEx(hHookKeyBoard, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(hHookMouse, nCode, wParam, lParam);

	if (GetActiveWindow() != hWndDraw && DRAWING && wParam == WM_LBUTTONUP){
		SendMessage(_hWnd, WM_KEYDOWN, VK_CONTROL, 0);
		DRAWING = !DRAWING;
	}

	return CallNextHookEx(hHookMouse, nCode, wParam, lParam);
}

EXPORT void InstallHook_Draw(HWND hWnd)
{
	if (hHookKeyBoard == NULL){
		_hWnd = hWnd;
		DRAWING = FALSE;
		hHookKeyBoard = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyBoardProc, hInstLib, 0);
	}

	if (hHookMouse == NULL){
		_hWnd = hWnd;
		DRAWING = FALSE;
		hHookMouse = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)MouseProc, hInstLib, 0);
	}
}

EXPORT void UninstallHook_Draw()
{
	if (hHookKeyBoard != NULL){

		UnhookWindowsHookEx(hHookKeyBoard);
		hHookKeyBoard = NULL;
	}

	if (hHookMouse != NULL){

		UnhookWindowsHookEx(hHookMouse);
		hHookMouse = NULL;
	}
}

EXPORT void UpdateDraw()
{
	DRAWING = FALSE;
	SendMessage(_hWnd, WM_KEYUP, VK_SHIFT, 0);
}

EXPORT void  GetHWND(HWND hWnd)
{
	hWndDraw = hWnd;
}