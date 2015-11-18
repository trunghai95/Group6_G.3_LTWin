// DLL_ControlMouse.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include<Windows.h>
#include<sstream>
#include <atlstr.h>
#include"Mouselogger.h"
#include "../IDs.h"
#define EXPORT  __declspec(dllexport)
#define IMPORT __declspec(dllimport)

HHOOK hHook = NULL;
HHOOK hHook2 = NULL;
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
void replayMouse();
void replayMouse(void * data);
void deleteInputMouse(INPUT *mouse);
WPARAM hookedParam[] = { WM_LBUTTONDOWN, WM_LBUTTONUP,
						WM_RBUTTONDOWN, WM_RBUTTONUP,
						WM_MBUTTONDOWN, WM_MBUTTONUP,
						WM_MOUSEWHEEL,
						WM_MOUSEMOVE };

LRESULT CALLBACK KBControlMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0 || GetActiveWindow() == hWnd)
		return CallNextHookEx(hHook, nCode, wParam, lParam);
	if (wParam == 'R' && HIWORD(GetKeyState(VK_CONTROL)) && HIWORD(GetKeyState(VK_SHIFT)))
	{
		mouseSequence->clear();
		delayTime->clear();
		record = true;
		return 1;
	}
	if (wParam == 'S' && HIWORD(GetKeyState(VK_CONTROL)) && HIWORD(GetKeyState(VK_SHIFT)))
	{
		record = false;
		return 1;
	}
	if (wParam == 'P' && ((GetKeyState(VK_CONTROL) & 0x8000) != 0) && ((GetKeyState(VK_SHIFT) & 0x8000) != 0) && !record)
	{
		HANDLE handle = (HANDLE)_beginthread(replayMouse, 0, NULL); // create thread
		//WaitForSingleObject(handle, INFINITE);
		//replayMouse();
		return 1;
	}

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
bool isHookMouse(WPARAM wParam)
{

	for (int i = 0; i < 8; ++i)
		if (wParam == hookedParam[i])
			return true;
	return false;
}
LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0 && record)
	{
		MouseMessage message;
		message.wParam = wParam;
		message.lParam = *((MSLLHOOKSTRUCT*)lParam);
		if ((*mouseSequence).size() == 0)
		{
			t1 = clock();
			(*delayTime).push_back(0);
		}
		else
		{
			t2 = clock();
			(*delayTime).push_back(t2 - t1);
			t1 = t2;
		}
		if (isHookMouse(wParam))
			(*mouseSequence).push_back(message);
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}
void mywait(clock_t time)
{
	clock_t a = clock();
	clock_t b;
	while (1)
	{
		b = clock();
		if (b - a > time)
			break;
	}
}
void replayMouse(void * data)
{
	replayMouse();
	_endthread();
}
void replayMouse()
{
	if (mouseSequence->size() <= 0)
	{
		MessageBox(hWnd, L"khong co du lieu", L"", NULL);
		return;
	}
	for (int i = 0; i < mouseSequence->size(); ++i)
	{
		mywait((*delayTime)[i]);
		MSLLHOOKSTRUCT mh = (*mouseSequence)[i].lParam;
		{
			INPUT *mouse = new INPUT;
			mouse->type = INPUT_MOUSE;
			mouse->mi.dx = mh.pt.x;
			mouse->mi.dy = mh.pt.y;
			SetCursorPos(mh.pt.x, mh.pt.y);

			switch ((*mouseSequence)[i].wParam)
			{
			case WM_LBUTTONDOWN:
				mouse->mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
				mouse->mi.mouseData = 0;
				break;
			case WM_LBUTTONUP:
				mouse->mi.dwFlags = MOUSEEVENTF_LEFTUP;
				mouse->mi.mouseData = 0;
				break;
			case WM_RBUTTONDOWN:
				mouse->mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
				mouse->mi.mouseData = 0;
				break;
			case WM_RBUTTONUP:
				mouse->mi.dwFlags = MOUSEEVENTF_RIGHTUP;
				mouse->mi.mouseData = 0;
				break;
			case WM_MBUTTONDOWN:
				mouse->mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
				mouse->mi.mouseData = 0;
				break;
			case WM_MBUTTONUP:
				mouse->mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
				mouse->mi.mouseData = 0;
				break;
			case WM_MOUSEWHEEL:
				mouse->mi.dwFlags = MOUSEEVENTF_WHEEL;
				mouse->mi.mouseData = mh.mouseData;
				break;
			case WM_MOUSEHWHEEL:
				mouse->mi.dwFlags = MOUSEEVENTF_HWHEEL;
				mouse->mi.mouseData = mh.mouseData;
				break;
			default:
				mouse->mi.dwFlags = mh.flags;
				mouse->mi.mouseData = mh.mouseData;
			}

			mouse->mi.time = mh.time;
			mouse->mi.dwExtraInfo = mh.dwExtraInfo;
			SendInput(1, mouse, sizeof(INPUT));

			deleteInputMouse(mouse);
		}
	}
}
EXPORT void InstallCtrlMouseHook(HWND hWndApp)
{
	hWnd = hWndApp;
	if (hHook == NULL)
		hHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KBControlMouseProc, hInstLib, 0);
	if (hHook2 == NULL)
	{
		mouseSequence = new vector<MouseMessage>();
		delayTime = new vector<clock_t>();
		hHook2 = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)MouseHookProc, hInstLib, 0);
	}
}

EXPORT void UninstallCtrlMouseHook()
{
	if (hHook != NULL){
		UnhookWindowsHookEx(hHook);
		hHook = NULL;
	}
	if (hHook2 != NULL)
	{
		UnhookWindowsHookEx(hHook);
		hHook = NULL;
	}
}

EXPORT void UpdateData(INT dir[], INT but[], INT spd)
{
	memcpy(Direction, dir, 4 * sizeof(INT));
	memcpy(Button, but, 5 * sizeof(INT));
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