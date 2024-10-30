#include "TraySet.hpp"

HotkeySetting g_hotkeySetting = { MOD_CONTROL | MOD_ALT, 'D' };
void SaveHotkeySettingToFile(const HotkeySetting& setting, const wchar_t* filename) {
    std::ofstream file(filename, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&setting), sizeof(HotkeySetting));
        file.close();
    }
}
void LoadHotkeySettingFromFile(const HotkeySetting& setting, const wchar_t* filename) {
    std::ifstream file(filename, std::ios::binary);
    if (file.is_open()) {
        file.read((char*)(&setting), sizeof(HotkeySetting));
        file.close();
    }
}
LRESULT CALLBACK SettingsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HotkeySetting newHotkeySetting = g_hotkeySetting; // new hotkey
    static UINT g_modifiers = 0;
    static HWND g_hwndTextBox = NULL;
    static HWND g_hwndButton = NULL;
    switch (msg) {
    case WM_CREATE: {
        g_hwndTextBox = CreateWindowExA(
            WS_EX_CLIENTEDGE, // add edge of textbox
            (LPCSTR)"Static", (LPCSTR)"Input HotKey and Holding Until Save",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_READONLY, 
            10, 10, 260, 20,
            hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);
        g_hwndButton = CreateWindowExA(
            0,
            (LPCSTR)"BUTTON", (LPCSTR)"Save Hotkey",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            10, 40, 260, 30,
            hwnd, (HMENU)2, GetModuleHandle(NULL), NULL);
        break;
    }
    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case 2: // save button click
            g_hotkeySetting = newHotkeySetting;
            SaveHotkeySettingToFile(g_hotkeySetting, L"hotkey.settings");
            break;
        }
        break;
    }
    case WM_KEYDOWN: {
        if ((lParam & 0x40000000) == 0) {//GetFocus() == g_hwndTextBox && 
            if (g_modifiers == 0) { // the first key down -> clear text
                SetWindowTextA(g_hwndTextBox, (LPCSTR)"\0");
            }
            switch (wParam) {
                case VK_SHIFT: g_modifiers |= MOD_SHIFT; break;
                case VK_CONTROL: g_modifiers |= MOD_CONTROL; break;
                case VK_MENU: g_modifiers |= MOD_ALT; break;
                case VK_LWIN: g_modifiers |= MOD_WIN; break;
                case VK_RWIN: g_modifiers |= MOD_WIN; break;
                default: {
                    newHotkeySetting.uModifiers = g_modifiers;
                    newHotkeySetting.uVirtKey = LOWORD(wParam);
                }
            }
            CHAR szHotkeyText[256];
            GetWindowTextA(g_hwndTextBox, szHotkeyText, 256);   // get nowText
            int i = _tcslen(szHotkeyText);
            //for (; i < 256 && szHotkeyText[i] != '\0'; i++) { ; }
            if (i != 0) { szHotkeyText[i] = '+'; i += 1; }
            GetKeyNameTextA(lParam, (LPSTR)(szHotkeyText + i), 256); //add new text after the old
            SetWindowTextA(g_hwndTextBox, (LPCSTR)szHotkeyText); // show full text
        }
        break;
    }
    case WM_KEYUP: {    //cancel the up key (but the text is wrong now, deal with the text is hard)
        if (1 | GetFocus() == g_hwndTextBox) {
            switch (wParam) {
            case VK_SHIFT: g_modifiers &= ~MOD_SHIFT; break;
            case VK_CONTROL: g_modifiers &= ~MOD_CONTROL; break;
            case VK_MENU: g_modifiers &= ~MOD_ALT; break;
            case VK_LWIN: g_modifiers &= ~MOD_WIN; break;
            case VK_RWIN: g_modifiers &= ~MOD_WIN; break;
            }
            if (wParam == VK_SHIFT || wParam == VK_CONTROL || wParam == VK_MENU) {
                SetWindowTextA(g_hwndTextBox, (LPCSTR)"\0");
                return 0;
            }
        }
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostMessage(main_hwnd, WM_USER_HKFLASH, 0, 0);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
LRESULT CALLBACK AboutWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HBITMAP hIconBitmap;
    static HFONT hLinkFont;
    static HFONT hStdFont;
    static RECT githubRect;
    static RECT emailRect;
    static HCURSOR hHandCursor; 
    static HCURSOR hArrowCursor;

    switch (msg) {
    case WM_CREATE: {
        hIconBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1));
        LOGFONT logFont;    // font set
        ZeroMemory(&logFont, sizeof(logFont));
        logFont.lfHeight = -MulDiv(SMALLFONT_SIZE, GetDeviceCaps(GetDC(hwnd), LOGPIXELSY), 72);
        logFont.lfUnderline = TRUE; 
        _tcscpy_s(logFont.lfFaceName, _T("宋体")); 
        hLinkFont = CreateFontIndirect(&logFont);
        ZeroMemory(&logFont, sizeof(logFont));
        logFont.lfHeight = -MulDiv(BIGFONT_SIZE, GetDeviceCaps(GetDC(hwnd), LOGPIXELSY), 72);
        _tcscpy_s(logFont.lfFaceName, _T("楷体"));
        hStdFont = CreateFontIndirect(&logFont);
        hHandCursor = LoadCursor(NULL, IDC_HAND);
        hArrowCursor = LoadCursor(NULL, IDC_ARROW);
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        HDC hdcMem = CreateCompatibleDC(hdc);
        SelectObject(hdcMem, hIconBitmap);
        BitBlt(hdc, 330, 40, 100, 100, hdcMem, 0, 0, SRCCOPY);
        DeleteDC(hdcMem);
        HFONT hOldFont = (HFONT)SelectObject(hdc, hStdFont);
        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, TRANSPARENT);
        RECT rect = { 10, BIGFONT_SIZE + 5, 300, BIGFONT_SIZE *16 };
        DrawText(hdc, TEXT_ABOUT, -1, &rect, DT_LEFT | DT_WORDBREAK);

        HFONT hOldLinkFont = (HFONT)SelectObject(hdc, hLinkFont);
        SetTextColor(hdc, RGB(0, 0, 255));
        githubRect.left = 10 + 5 * BIGFONT_SIZE;
        githubRect.top = BIGFONT_SIZE *12 + 3;
        githubRect.right = githubRect.left + _tcslen(TEXT_GITHUB) * SMALLFONT_SIZE;
        githubRect.bottom = githubRect.top + SMALLFONT_SIZE;
        TextOut(hdc, githubRect.left, githubRect.top, TEXT_GITHUB, _tcslen(TEXT_GITHUB));
        emailRect.left = 10 + 4 * BIGFONT_SIZE;
        emailRect.top = BIGFONT_SIZE *13 +5;
        emailRect.right = emailRect.left + _tcslen(TEXT_EMAIL) * SMALLFONT_SIZE;
        emailRect.bottom = emailRect.top + SMALLFONT_SIZE;
        TextOut(hdc, emailRect.left, emailRect.top, TEXT_EMAIL, _tcslen(TEXT_EMAIL));

        SelectObject(hdc, hOldFont);
        SelectObject(hdc, hOldLinkFont);

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_SETCURSOR: {
        if (LOWORD(lParam) == HTCLIENT) {   // turn cursor when it on link
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hwnd, &pt);
            if (PtInRect(&githubRect, pt) || PtInRect(&emailRect, pt)) { SetCursor(hHandCursor); }
            else { SetCursor(hArrowCursor); }
            return TRUE;
        }
        break;
    }
    case WM_LBUTTONDOWN: {
        int xPos = LOWORD(lParam);
        int yPos = HIWORD(lParam);
        POINT pMouse = { xPos ,yPos};
        if (PtInRect(&githubRect, pMouse)) {    // click link to visit it
            ShellExecute(hwnd, _T("open"), TEXT_GITHUB, NULL, NULL, SW_SHOW);
        }
        else if (PtInRect(&emailRect, pMouse)) {
            ShellExecute(hwnd, _T("open"), TEXT_EMAIL, NULL, NULL, SW_SHOW);
        }
        break;
    }
    case WM_DESTROY:
        if (hLinkFont) {DeleteObject(hLinkFont);}
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

HWND CreateAboutWindow(HWND hwndParent) {
    WNDCLASSA wc = {};
    wc.lpfnWndProc = AboutWndProc;
    wc.hInstance = 0;
    wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
    wc.lpszClassName = "AboutWindowClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassA(&wc);
    HWND hwndSettings = CreateWindowExA(
        0,
        "AboutWindowClass",
        "关于",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 400,
        hwndParent,NULL,GetModuleHandle(NULL),NULL);
    ShowWindow(hwndSettings, SW_SHOW);
    MSG msg; while (GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return hwndSettings;
}
HWND CreateSettingsWindow(HWND hwndParent) {
    WNDCLASSA wc = {};
    wc.lpfnWndProc = SettingsWndProc;
    wc.hInstance = 0;
    wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
    wc.lpszClassName = "SettingsWindowClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassA(&wc);
    HWND hwndSettings = CreateWindowExA(
        0,
        "SettingsWindowClass",
        "设置快捷键",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
        hwndParent,NULL,GetModuleHandle(NULL),NULL);
    ShowWindow(hwndSettings, SW_SHOW);
    MSG msg; while (GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return hwndSettings;
}
void CaptureAndStartShow(HWND hwnd) {
    UnregisterHotKey(main_hwnd, 1); // when select rect, unable hotkey
    FloatWnd* p_fltwnd = (FloatWnd*)malloc(sizeof(FloatWnd));
    p_fltwnd->is_finished_select_rect = false;  // if still false after select, then jmp over show
    SelectCaptureRect(p_fltwnd);
    if (p_fltwnd->is_finished_select_rect) {
        std::thread captureThread(CaptureAndDisplayScreen, p_fltwnd);   //new thread to show floatwnd
        captureThread.detach();
    }
    RegisterHotKey(main_hwnd, 1, g_hotkeySetting.uModifiers, g_hotkeySetting.uVirtKey);
}
HMENU hMenu;
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    LoadHotkeySettingFromFile(g_hotkeySetting, L"hotkey.settings");
        HINSTANCE hinstance = (HINSTANCE)GetWindowLongPtrA(hwnd, GWLP_HINSTANCE);
    static NOTIFYICONDATAA nid;
    UINT WM_TASKBARCREATED;
    POINT pt;
    int menuID;
    WM_TASKBARCREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));
    switch (msg)
    {
    case WM_CREATE:
        nid.cbSize = sizeof(nid);// set menu item
        nid.hWnd = hwnd;
        nid.uID = 0;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_USER;
        nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
        lstrcpyA(nid.szTip, APP_NAME);
        Shell_NotifyIconA(NIM_ADD, &nid);
        hMenu = CreatePopupMenu();
        AppendMenu(hMenu, MF_STRING, ID_SETTING, TEXT("设置快捷键"));
        AppendMenu(hMenu, MF_STRING, ID_ABOUT, TEXT("关于"));
        AppendMenu(hMenu, MF_STRING, ID_QUIT, TEXT("退出"));
        Shell_NotifyIconA(NIM_ADD, &nid);
        break;
    case WM_USER:
        if (lParam == WM_LBUTTONDOWN) { 
            CaptureAndStartShow(hwnd); 
        }
        if (lParam == WM_RBUTTONDOWN){
            GetCursorPos(&pt);
            ::SetForegroundWindow(hwnd);// avoid keep menu showing when click others
            //EnableMenuItem(hMenu, ID_SHOW, MF_GRAYED);
            menuID = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hwnd, NULL);
            if (menuID == ID_SETTING) {
                std::thread captureThread(CreateSettingsWindow, hwnd);
                captureThread.detach();
            }
            if (menuID == ID_ABOUT) {
                std::thread captureThread(CreateAboutWindow, hwnd);
                captureThread.detach();
            }//MessageBoxA(hwnd, APP_ABOUT, APP_NAME, MB_OK);
            if (menuID == ID_QUIT) { SendMessage(hwnd, WM_CLOSE, wParam, lParam); }
            //if (menuID == 0) PostMessage(hwnd, WM_LBUTTONDOWN, NULL, NULL);
        }
        break;
    case WM_USER_HKFLASH: {
        UnregisterHotKey(hwnd, 1); RegisterHotKey(hwnd, 1, g_hotkeySetting.uModifiers, g_hotkeySetting.uVirtKey);
        break;
    }
    case WM_DESTROY:
        Shell_NotifyIconA(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;
    case WM_HOTKEY:
        if (wParam == 1) {
            CaptureAndStartShow(hwnd);
        }
        break;
    default:
        if (msg == WM_TASKBARCREATED){
            SendMessage(hwnd, WM_CREATE, wParam, lParam);
        }    
        else {
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        break;
    }
    return 0;
}