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

	if (vkCode <= VK_F12 && vkCode >= VK_F1)
	{
		wsprintf(buffer, L"F%d", vkCode - VK_F1 + 1);
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
	case VK_BACK:
		wsprintf(buffer, L"Backspace");
		break;
	case VK_ESCAPE:
		wsprintf(buffer, L"Escape");
		break;
	case VK_SPACE:
		wsprintf(buffer, L"Space bar");
		break;
	case VK_PRIOR:
		wsprintf(buffer, L"Page up");
		break;
	case VK_NEXT:
		wsprintf(buffer, L"Page down");
		break;
	case VK_END:
		wsprintf(buffer, L"End");
		break;
	case VK_HOME:
		wsprintf(buffer, L"Home");
		break;
	case VK_INSERT:
		wsprintf(buffer, L"Insert");
		break;
	case VK_DELETE:
		wsprintf(buffer, L"Delete");
		break;
	case VK_MULTIPLY:
		wsprintf(buffer, L"*");
		break;
	case VK_ADD:
		wsprintf(buffer, L"+");
		break;
	case VK_DIVIDE:
		wsprintf(buffer, L"/");
		break;
	case VK_SUBTRACT:
		wsprintf(buffer, L"-");
		break;
	case VK_DECIMAL:
		wsprintf(buffer, L".");
		break;
	case VK_OEM_1:
		wsprintf(buffer, L";:");
		break;
	case VK_OEM_PLUS:
		wsprintf(buffer, L"=+");
		break;
	case VK_OEM_COMMA:
		wsprintf(buffer, L",<");
		break;
	case VK_OEM_MINUS:
		wsprintf(buffer, L"-_");
		break;
	case VK_OEM_PERIOD:
		wsprintf(buffer, L".>");
		break;
	case VK_OEM_2:
		wsprintf(buffer, L"/?");
		break;
	case VK_OEM_3:
		wsprintf(buffer, L"`~");
		break;
	case VK_OEM_4:
		wsprintf(buffer, L"[{");
		break;
	case VK_OEM_5:
		wsprintf(buffer, L"\\|");
		break;
	case VK_OEM_6:
		wsprintf(buffer, L"]}");
		break;
	case VK_OEM_7:
		wsprintf(buffer, L"'\"");
		break;
	}

	return buffer;
}