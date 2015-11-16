// DLL_ControlMouse.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "../IDs.h"
#define EXPORT  __declspec(dllexport)
#define IMPORT __declspec(dllimport)

HHOOK hHook = NULL;
HWND hWnd = NULL;
INT Direction[4] = { VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN }; //default values
INT Button[5] = { 'A', 'D', 'S', 'W', 'X' };
INT speed = 10;
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

	KBDLLHOOKSTRUCT* str = (KBDLLHOOKSTRUCT*)lParam;

	if (HIWORD(GetKeyState(VK_CONTROL)) && HIWORD(GetKeyState(VK_SHIFT)))
	{
		if (wParam == WM_KEYDOWN && str->vkCode == activateKey)
		{
			onMouse = !onMouse;
			MessageBeep(MB_ICONWARNING);
			return 1;
		}

		return CallNextHookEx(hHook, nCode, wParam, lParam);
	}

	if (onMouse && IsInCustomKeys(str->vkCode))
	{
		if (wParam == WM_KEYDOWN)
		{
			MoveMouse(hWnd, str->vkCode);
			ClickMouse(str->vkCode);
		}
		else
			ReleaseMouse(str->vkCode);
		return 1;
	}

	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

EXPORT void InstallCtrlMouseHook(HWND hWndApp)
{
	hWnd = hWndApp;
	if (hHook != NULL) return;
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KBControlMouseProc, hInstLib, 0);
}

EXPORT void UninstallCtrlMouseHook()
{
	if (hHook == NULL) return;
	UnhookWindowsHookEx(hHook);
	hHook = NULL;
}

EXPORT void UpdateData(INT dir[], INT but[], INT spd)
{
	for (int i = 0; i < 4; i++)
		Direction[i] = dir[i];
	for (int i = 0; i < 5; i++)
		Button[i] = but[i];
	speed = spd;
}

void MoveMouse(HWND hWnd, WPARAM wParam)
{
	POINT pt;

	GetCursorPos(&pt);

	ScreenToClient(hWnd, &pt);

	if (wParam == Direction[LEFT]) // Left
		pt.x -= speed;
	else if (wParam == Direction[UP]) // Up
		pt.y -= speed;
	else if (wParam == Direction[RIGHT]) // Right
		pt.x += speed;
	else if (wParam == Direction[DOWN]) // Down
		pt.y += speed;

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
	if (wParam == Button[LBUTTON]) //Left click
		SendEventClick(MOUSEEVENTF_LEFTDOWN, 0);
	else if (wParam == Button[RBUTTON]) //right click
		SendEventClick(MOUSEEVENTF_RIGHTDOWN, 0);
	else if (wParam == Button[MBUTTON]) // middle click
		SendEventClick(MOUSEEVENTF_MIDDLEDOWN, 0);
	else if (wParam == Button[WHEELUP]) // roll up
		SendEventClick(MOUSEEVENTF_WHEEL, WHEEL_DELTA);
	else if (wParam == Button[WHEELDOWN]) // roll down
		SendEventClick(MOUSEEVENTF_WHEEL, -WHEEL_DELTA);
}

void ReleaseMouse(WPARAM wParam)
{
	if (wParam == Button[LBUTTON]) //Left up
		SendEventClick(MOUSEEVENTF_LEFTUP,0);
	else if (wParam == Button[RBUTTON]) //right up
		SendEventClick(MOUSEEVENTF_RIGHTUP,0);
	else if (wParam == Button[MBUTTON]) // middle up
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