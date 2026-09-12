#pragma once
#include <windows.h>
#include <iostream>
#include <iomanip>

namespace ConsoleFunctions
{
    void goFullscreen()
    {
        // key events to go full screen
        keybd_event(VK_MENU, 0x38, 0, 0);
        keybd_event(VK_RETURN, 0x1c, 0, 0);
        keybd_event(VK_RETURN, 0x1c, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0); 
        
        HWND hwnd = GetForegroundWindow();
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
        SetFocus(hwnd);
    }

    void desyncSDTIO()
    {
        std::ios_base::sync_with_stdio(false);
        std::cin.tie(nullptr);
    }

    void setPrecision(std::streamsize precision)
    {
        SetConsoleOutputCP(CP_UTF8);
        std::cout << std::setprecision(precision);
        std::cout << "\033[?25l" << std::flush;
    }
}