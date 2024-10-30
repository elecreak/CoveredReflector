#include "FloatWnd.hpp"
// capture from g_hCaptureWindow and show on floatwnd
void UpdateCapturedWindow(FloatWnd* p_fltwnd) {
    if (p_fltwnd->g_hCaptureWindow == NULL) return;
    HDC hdcScreen = GetDC(NULL);
    HDC hdcCapture = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmCapture = CreateCompatibleBitmap(hdcScreen,
        p_fltwnd->g_selectionRect.right - p_fltwnd->g_selectionRect.left,
        p_fltwnd->g_selectionRect.bottom - p_fltwnd->g_selectionRect.top);
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcCapture, hbmCapture);
    HDC hdcSrc = GetDC(p_fltwnd->g_hCaptureWindow);// get target wnd dc
    BitBlt(hdcCapture, 0, 0,
        p_fltwnd->g_selectionRect.right - p_fltwnd->g_selectionRect.left,
        p_fltwnd->g_selectionRect.bottom - p_fltwnd->g_selectionRect.top,
        hdcSrc, p_fltwnd->g_selectionRect.left, p_fltwnd->g_selectionRect.top, 
        CAPTUREBLT | SRCCOPY);
    ReleaseDC(p_fltwnd->g_hCaptureWindow, hdcSrc);
    POINT ptSrc = { 0, 0 };
    SIZE size = { p_fltwnd->g_selectionRect.right - p_fltwnd->g_selectionRect.left,
        p_fltwnd->g_selectionRect.bottom - p_fltwnd->g_selectionRect.top };
    RECT rect;
    GetWindowRect(p_fltwnd->g_hCaptureWindow, &rect);
    POINT ptDst = { p_fltwnd->g_selectionRect.top, p_fltwnd->g_selectionRect.left };
    UpdateLayeredWindow(p_fltwnd->hwndFloat, hdcScreen, NULL, &size, hdcCapture,
        &ptSrc, 0, &(p_fltwnd->blendFunc), LWA_ALPHA);
    SelectObject(hdcCapture, hbmOld);
    DeleteDC(hdcCapture);
    ReleaseDC(NULL, hdcScreen);
    DeleteObject(hbmCapture);
}
LRESULT CALLBACK SelectionWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    FloatWnd* p_fltwnd = (FloatWnd*)GetPropA(hwnd, "p_fltwnd");
    switch (msg) {
    case WM_LBUTTONDOWN:    // left down
        GetCursorPos((LPPOINT) & (p_fltwnd->g_selectionRect));
        p_fltwnd->g_selectionRect.right = p_fltwnd->g_selectionRect.left;
        p_fltwnd->g_selectionRect.bottom = p_fltwnd->g_selectionRect.top;
        break;
    case WM_MOUSEMOVE:
        if (wParam & MK_LBUTTON) {  // move when left down
            POINT pt;
            GetCursorPos(&pt);
            p_fltwnd->g_selectionRect.right = pt.x;
            p_fltwnd->g_selectionRect.bottom = pt.y;
            InvalidateRect(hwnd, NULL, TRUE);   // send paint msg
        }
        break;
    case WM_LBUTTONUP:  // left up, select rect finish
        ShowWindow(hwnd, SW_HIDE);  // finish, so hide the wnd
        break;
    case WM_RBUTTONDOWN:    // right down, cancel selecting
        ShowWindow(hwnd, SW_HIDE);
        break;
    case WM_PAINT:{
        HDC hdcScreen = GetDC(NULL);
        HDC hdc = CreateCompatibleDC(hdcScreen);
        RECT rectClient;
        GetClientRect(hwnd, &rectClient);
        BITMAPINFO bmi;
        memset(&bmi, 0, sizeof(BITMAPINFO));
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = rectClient.right;
        bmi.bmiHeader.biHeight = -rectClient.bottom; // draw from top to bottom
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32; // 32 bit ARGB
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage = 0;
        void* pBits;
        HBITMAP hbm = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
        if (hbm) SelectObject(hdc, hbm);
        FillRect(hdc, &rectClient, (HBRUSH)GetStockObject(NULL_BRUSH)); // clear all things in hdc
        //begin to draw rect
        HRGN hRgn = CreateRectRgnIndirect(&(p_fltwnd->g_selectionRect));
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
        FrameRgn(hdc, hRgn, hBrush, 1, 1); // draw the rect in hdc
        //FillRect(hdc, &(p_fltwnd->g_selectionRect), hBrush);
        DeleteObject(hBrush);
        DeleteObject(hRgn);
        // set full screen alpha (background color)
        BLENDFUNCTION blendFunc = { 0 };
        blendFunc.BlendOp = AC_SRC_OVER;
        blendFunc.SourceConstantAlpha = 100; // alpha
        blendFunc.AlphaFormat = AC_SRC_OVER;

        POINT ptSrc = { 0, 0 };
        SIZE size = { rectClient.right, rectClient.bottom };
        UpdateLayeredWindow(hwnd, hdcScreen, NULL, &size, hdc, &ptSrc, 0, &blendFunc, LWA_ALPHA);
        ReleaseDC(NULL, hdcScreen);
        DeleteObject(hbm);
        DeleteDC(hdc);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    //return 0;
}

LRESULT CALLBACK FloatWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    FloatWnd* p_fltwnd = (FloatWnd*)GetPropA(hwnd, "p_fltwnd");
    int zDelta = 0;
    int next_alpha = 100;
    //if (p_fltwnd->ctrl_mux == 0) {    // for turn to control the targetwnd on floatwnd
        switch (uMsg) {
        case WM_TIMER:
            UpdateCapturedWindow(p_fltwnd);
            break;
        case WM_NCHITTEST:
            return HTCAPTION;   // for set all floatwnd as topArea to enable move by mouse
            break;
        case WM_NCLBUTTONDBLCLK:    // double click to close
            if (GetKeyState(VK_CONTROL) & 0x8000) { //if ctrl + dblclk, set targetwnd top
                SetForegroundWindow(p_fltwnd->g_hCaptureWindow);
            }
            DestroyWindow(hwnd);
            break;
        case WM_MOUSEWHEEL: // for scoll mouse to change alpha
            zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            next_alpha = p_fltwnd->blendFunc.SourceConstantAlpha;
            next_alpha += (int)(zDelta * 2 / 120.0f);
            if (next_alpha < 30) { next_alpha = 30; }   //limit alpha in 30-255, avoid over8bit
            else if (next_alpha > 255) { next_alpha = 255; }
            p_fltwnd->blendFunc.SourceConstantAlpha = next_alpha;
            break;
        /*case WM_KEYDOWN:  // for turn to control the targetwnd on floatwnd
            if (wParam == (VK_MENU) && GetKeyState(VK_CONTROL)) {
                p_fltwnd->ctrl_mux = 1;
            }
            break;*/
        case WM_DESTROY:
            KillTimer(hwnd, 1);
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    //}
    /*else if (p_fltwnd->ctrl_mux == 1) {   // for turn to control the targetwnd on floatwnd
        switch (uMsg) {
        case WM_MOUSEWHEEL:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MOUSEMOVE:
        case WM_KEYUP:
            PostMessage(p_fltwnd->g_hCaptureWindow, uMsg, wParam, lParam);
            break;
        case WM_TIMER:
            UpdateCapturedWindow(p_fltwnd);
            break;
        case WM_DESTROY:
            KillTimer(hwnd, 1); 
            PostQuitMessage(0);
            break;
        case WM_KEYDOWN:
            PostMessage(p_fltwnd->g_hCaptureWindow, uMsg, wParam, lParam);
            if (wParam == (VK_MENU) && GetKeyState(VK_CONTROL)) {
                p_fltwnd->ctrl_mux = 0;
            }
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }*/
    return 0;
}
// find out top wnd on lefttop point of floatwnd by search by z-order 
void SearchWindow(FloatWnd* p_fltwnd) {
    RECT rect;
    DWORD flags;    BYTE tou;
    do{
        if (GetTopWindow(p_fltwnd->g_hCaptureWindow) != NULL) {
            HWND f_hwnd = p_fltwnd->g_hCaptureWindow;
            p_fltwnd->g_hCaptureWindow = GetWindow(p_fltwnd->g_hCaptureWindow, GW_CHILD);
            p_fltwnd->g_hCaptureWindow = f_hwnd;
        }
        GetWindowRect(p_fltwnd->g_hCaptureWindow, &rect);
        POINT pt;
        pt.x = p_fltwnd->g_selectionRect.left;
        pt.y = p_fltwnd->g_selectionRect.top;
        if (PtInRect(&rect, pt)) {
            GetLayeredWindowAttributes(p_fltwnd->g_hCaptureWindow, NULL, &tou, &flags);
            bool bVisible = (::GetWindowLong(p_fltwnd->g_hCaptureWindow, GWL_STYLE) & WS_VISIBLE) != 0;
            if (bVisible) { // only visible wnd is the right wnd
                p_fltwnd->g_selectionRect.bottom -= rect.top + 8;
                p_fltwnd->g_selectionRect.top -= rect.top + 8;
                p_fltwnd->g_selectionRect.left -= rect.left + 8;
                p_fltwnd->g_selectionRect.right -= rect.left + 8;
                break;
            }
            //if ((int)p_fltwnd->g_hCaptureWindow > 0x20000) {}
        }
    } while (NULL != (p_fltwnd->g_hCaptureWindow = GetWindow(p_fltwnd->g_hCaptureWindow, GW_HWNDNEXT)));
}


void SelectCaptureRect(FloatWnd* p_fltwnd) {
    const char* propKey = "p_fltwnd";
    p_fltwnd->ctrl_mux = 0;
    p_fltwnd->blendFunc = { AC_SRC_OVER,0,254, AC_SRC_OVER };
    // 创建选择区域的窗口
    HWND hwndSelection = CreateWindowExA(WS_EX_LAYERED | WS_EX_TOPMOST, "STATIC", "",
        WS_POPUP | WS_VISIBLE,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, GetModuleHandle(NULL), NULL);
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
    SendMessage(hwndSelection, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    SendMessage(hwndSelection, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    SetWindowLong(hwndSelection, GWL_EXSTYLE,
        GetWindowLong(hwndSelection, GWL_EXSTYLE) & (~WS_EX_TRANSPARENT));
    SetWindowLongPtr(hwndSelection, GWLP_WNDPROC, (LONG_PTR)SelectionWndProc);
    SetPropA(hwndSelection, propKey, (HANDLE)p_fltwnd);
    // set p_fltwnd as propKey of hselectwnd, for using p_fltwnd in proc callback func
    SetWindowPos(hwndSelection, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    MSG msg;
    InvalidateRect(hwndSelection, NULL, TRUE);
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_RBUTTONDOWN) {
            break;
        }
        if (msg.message == WM_LBUTTONUP) {
            if (p_fltwnd->g_selectionRect.bottom != p_fltwnd->g_selectionRect.top &&
                p_fltwnd->g_selectionRect.left != p_fltwnd->g_selectionRect.right) {
                p_fltwnd->is_finished_select_rect = true;
            }
            break;
        }
    }
}
void CaptureAndDisplayScreen(FloatWnd* p_fltwnd) {
    const char* propKey = "p_fltwnd";
    MSG msg;
    p_fltwnd->hwndFloat = CreateWindowExA(WS_EX_LAYERED | WS_EX_TOPMOST, "STATIC", "",
        WS_POPUP | WS_VISIBLE,
        p_fltwnd->g_selectionRect.right, p_fltwnd->g_selectionRect.bottom,
        p_fltwnd->g_selectionRect.right - p_fltwnd->g_selectionRect.left,
        p_fltwnd->g_selectionRect.bottom - p_fltwnd->g_selectionRect.top,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    SetWindowLong(p_fltwnd->hwndFloat, GWL_EXSTYLE,
        GetWindowLong(p_fltwnd->hwndFloat, GWL_EXSTYLE) & (~WS_EX_TRANSPARENT));
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
    SendMessage(p_fltwnd->hwndFloat, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    SendMessage(p_fltwnd->hwndFloat, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    SetWindowLongPtr(p_fltwnd->hwndFloat, GWLP_WNDPROC, (LONG_PTR)FloatWindowProc);

    HWND hDsktp = GetDesktopWindow();
    p_fltwnd->g_hCaptureWindow = hDsktp;
    p_fltwnd->g_hCaptureWindow = GetTopWindow(p_fltwnd->g_hCaptureWindow);//GetWindow(hDsktp, GW_CHILD);
    SearchWindow(p_fltwnd);
    //p_fltwnd->g_hCaptureWindow = (HWND)3869022;   // for test
    SetTimer(p_fltwnd->hwndFloat, 1, 40, NULL); // set fps as 1000/40 = 25Hz
    SetPropA(p_fltwnd->hwndFloat, propKey, (HANDLE)p_fltwnd);   
    // set p_fltwnd as propKey in hwndFloat, for using p_fltwnd in proc callback func of hwndFloat
    while (GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}