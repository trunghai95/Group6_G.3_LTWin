#pragma once

#include "stdafx.h"
#include <string>

TCHAR* GetKeyName(int, TCHAR*);

TCHAR* GetKeyName(int vkCode, TCHAR* buffer)
{
	if (buffer == NULL)
		return buffer;

	buffer[0] = 0;

	if ((vkCode <= 'Z' && vkCode >= 'A') || (vkCode <= '9' && vkCode >= '0'))
	{
		wsprintf(buffer, L"%c", vkCode);
		return buffer;
	}

	if (vkCode <= VK_NUMPAD9 && vkCode >= VK_NUMPAD0)
	{
		wsprintf(buffer, L"Numpad %d", vkCode - VK_NUMPAD0);
		return buffer;
	}

	switch (vkCode)
	{
	case VK_CONTROL:
		wsprintf(buffer, L"Control");
		break;
	case VK_SHIFT:
		wsprintf(buffer, L"Shift");
		break;
	case VK_RETURN:
		wsprintf(buffer, L"Enter");
		break;
	case VK_TAB:
		wsprintf(buffer, L"Tab");
		break;
	case VK_UP:
		wsprintf(buffer, L"Up");
		break;
	case VK_DOWN:
		wsprintf(buffer, L"Down");
		break;
	case VK_LEFT:
		wsprintf(buffer, L"Left");
		break;
	case VK_RIGHT:
		wsprintf(buffer, L"Right");
		break;
	default:
		wsprintf(buffer, L"Chưa cài tới :v");
		break;
	}

	return buffer;
}