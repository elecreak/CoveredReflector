#ifndef _TRAYSET_HPP_
#define _TRAYSET_HPP_
#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h> 
#include <cstdio>
#include <thread>
#include <fstream>
#include <tchar.h>
#include <shellapi.h>
#include <wingdi.h>
#include "FloatWnd.hpp"
#include "resource.h"

#define ID_SETTING 1
#define ID_ABOUT 2
#define ID_QUIT 3
#define WM_USER_HKFLASH (WM_USER+1)
#define APP_NAME_EN (LPCSTR)"CoveredReflector"
#define APP_NAME (LPCSTR)"”≥¥∞"
#define TEXT_ABOUT (LPCSTR)"\n”≥¥∞ | CoveredReflector\n\n    ver 0.1.0 (beta)\n\n---\n\n\nGitHub:\nEmail:"
#define TEXT_GITHUB (LPCSTR)"https://github.com/elecreak/CoveredReflector"
#define TEXT_EMAIL (LPCSTR)"elecreak@outlook.com"
#define BIGFONT_SIZE 16 
#define SMALLFONT_SIZE 12 
typedef struct {
    UINT uModifiers;
    UINT uVirtKey;
} HotkeySetting;

extern HotkeySetting g_hotkeySetting;
extern HWND main_hwnd;
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif