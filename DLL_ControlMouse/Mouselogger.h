#pragma once
#pragma pack(1)
#include "stdafx.h"
#include<vector>
#include<time.h>
using namespace std;

struct MouseMessage
{
	WPARAM wParam;
	MSLLHOOKSTRUCT lParam;
};
#pragma data_seg()
static vector<MouseMessage>* mouseSequence = NULL;
static vector<clock_t>* delayTime = NULL;
static bool record = false;
static clock_t t1 = 0;
static clock_t t2 = 0;
#pragma comment(linker,"/SECTION:shared,RWS")