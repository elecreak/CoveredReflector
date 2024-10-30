#ifndef _FLOATWND_HPP_
#define _FLOATWND_HPP_
#include <Windows.h>
#include <fstream>
#include "TraySet.hpp"

typedef struct floatwnd {
    HWND g_hCaptureWindow = NULL;	//targetwnd
    HWND hwndFloat = NULL;
    RECT g_selectionRect;	// select rect
    BLENDFUNCTION blendFunc = { AC_SRC_OVER,0,254, AC_SRC_ALPHA };
    uint8_t ctrl_mux = 0;
    bool is_finished_select_rect;
}FloatWnd;

void CaptureAndDisplayScreen(FloatWnd* p_fltwnd);
void SelectCaptureRect(FloatWnd* p_fltwnd);

#endif