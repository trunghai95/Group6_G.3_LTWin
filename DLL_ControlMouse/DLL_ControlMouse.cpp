// DLL_ControlMouse.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#define EXPORT  __declspec(dllexport)
#define IMPORT __declspec(dllimport)
HHOOK hHook = NULL;
HWND hWnd = NULL;
INT Direction[4] = { VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN }; //default values
INT Button[5] = { 'A', 'D', 'S', 'W', 'X' };
extern HINSTANCE hInstLib;

bool onMouse = false;
const int activateKey = 'M'; // Activated by Ctrl + Shift + activateKey

void MoveMouse(HWND hWnd, WPARAM wParam);
void ClickMouse(WPARAM);
void ReleaseMouse(WPARAM);
bool IsInCustomKeys(WPARAM wParam);

LRESULT CALLBACK KBControlMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0 || GetActiveWindow() == hWnd)
		return CallNextHookEx(hHook, nCode, wParam, lParam);

	if (wParam == activateKey)
	{
		if (GetAsyncKeyState(wParam) && GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_SHIFT))
		{
			onMouse = !onMouse;
			MessageBeep(MB_ICONWARNING);
			return 1;
		}

		return CallNextHookEx(hHook, nCode, wParam, lParam);
	}

	if (onMouse && IsInCustomKeys(wParam))
	{
		if (GetAsyncKeyState(wParam))
		{
			MoveMouse(hWnd, wParam);
			ClickMouse(wParam);
		}
		else
			ReleaseMouse(wParam);
		return 1;
	}

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

EXPORT void UpdateData(INT dir[], INT but[])
{
	for (int i = 0; i < 4; i++)
		Direction[i] = dir[i];
	for (int i = 0; i < 5; i++)
		Button[i] = but[i];
}

void MoveMouse(HWND hWnd, WPARAM wParam)
{
	int repeat = 10;
	POINT pt;

	GetCursorPos(&pt);

	ScreenToClient(hWnd, &pt);

	if (wParam == Direction[0]) // Left
		pt.x -= repeat;
	if (wParam == Direction[1]) // Up
		pt.y -= repeat;
	if (wParam == Direction[2]) // Right
		pt.x += repeat;
	if (wParam == Direction[3]) // Down
		pt.y += repeat;

	ClientToScreen(hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

INPUT* getInputMouse()
{
	POINT pt;
	GetCursorPos(&pt);
	INPUT *mouse = new INPUT;
	mouse->type = INPUT_MOUSE;
	mouse->mi.dx = pt.x;
	mouse->mi.dy = pt.y;
	mouse->mi.mouseData = 0;
	mouse->mi.dwFlags = MOUSEEVENTF_ABSOLUTE;
	mouse->mi.time = 0;
	mouse->mi.dwExtraInfo = 0;
	return mouse;
}

void deleteInputMouse(INPUT *mouse)
{
	if (mouse)
	{
		delete mouse;
		mouse = NULL;
	}
}

void SendEventClick(INT Event, INT Data)
{
	INPUT* mouse = getInputMouse();
	mouse->mi.dwFlags = Event;
	mouse->mi.mouseData = Data;
	SendInput(1, mouse, sizeof(INPUT));
	deleteInputMouse(mouse);
}

void ClickMouse(WPARAM wParam)
{
	if (wParam == Button[0]) //Left click
		SendEventClick(MOUSEEVENTF_LEFTDOWN, 0);
	else if (wParam == Button[1]) //right click
		SendEventClick(MOUSEEVENTF_RIGHTDOWN, 0);
	else if (wParam == Button[2]) // middle click
		SendEventClick(MOUSEEVENTF_MIDDLEDOWN, 0);
	else if (wParam == Button[3]) // roll up
		SendEventClick(MOUSEEVENTF_WHEEL, WHEEL_DELTA);
	else if (wParam == Button[4]) // roll down
		SendEventClick(MOUSEEVENTF_WHEEL, -WHEEL_DELTA);
}

void ReleaseMouse(WPARAM wParam)
{
	if (wParam == Button[0]) //Left up
		SendEventClick(MOUSEEVENTF_LEFTUP,0);
	else if (wParam == Button[1]) //right up
		SendEventClick(MOUSEEVENTF_RIGHTUP,0);
	else if (wParam == Button[2]) // middle up
		SendEventClick(MOUSEEVENTF_MIDDLEUP,0);

}

bool IsInCustomKeys(WPARAM wParam)
{
	for (int i = 0; i < 4; i++)
		if (wParam == Direction[i])
			return TRUE;
	for (int i = 0; i < 5; i++)
		if (wParam == Button[i])
			return TRUE;
	return FALSE;
}