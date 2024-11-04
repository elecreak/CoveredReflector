#include "FloatWnd.hpp"
#include "TraySet.hpp"
#include "resource.h"

bool WriteBootStartUpToRegedit(){
    char filepath[128];
    HKEY hkey;
    GetModuleFileNameA(NULL, filepath, 128);
    RegCreateKeyExA(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0,NULL,REG_OPTION_NON_VOLATILE,KEY_WRITE,NULL,&hkey,NULL);
    RegSetValueExA(hkey,APP_NAME_EN,0,REG_SZ,reinterpret_cast<LPBYTE>((filepath)),
        sizeof(std::wstring::value_type) * (_tcslen(filepath) + 1));
    RegCloseKey(hkey);//RegDeleteKeyExW
    return true;
}
HWND main_hwnd;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WriteBootStartUpToRegedit();
    MSG msg;
    WNDCLASSA wndclass;
    HWND handle = FindWindowA(NULL, APP_NAME_EN);
    if (handle != NULL){
        //MessageBoxA(NULL,"Application is already running", APP_NAME_EN, MB_ICONERROR);
        //return 0;
        SendMessage(handle, WM_DESTROY, 0, 0);
    }
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = MainWndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = APP_NAME_EN;
    RegisterClassA(&wndclass);
    main_hwnd = CreateWindowExA(WS_EX_TOOLWINDOW,   //TOOLWINDOW to hide icon on taskbar
        APP_NAME_EN, APP_NAME_EN, WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, NULL, NULL);
    if (!RegisterHotKey(main_hwnd, 1, g_hotkeySetting.uModifiers, g_hotkeySetting.uVirtKey)) {// ×¢²á¿ì½Ý¼ü
        MessageBoxA(NULL, "Failed to register hotkey", "Error", MB_OK);
    }
    ShowWindow(main_hwnd, 0);
    UpdateWindow(main_hwnd);
    while (GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
