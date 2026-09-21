#pragma once

#include <iomanip>
#include <iostream>
#include <windows.h>

namespace ConsoleFunctions
{
    inline void goFullscreen()
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

    inline void setBackgroundBlack() {
        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
    }

    inline void desyncSTDIO()
    {
        std::ios_base::sync_with_stdio(false);
        std::cin.tie(nullptr);
    }

    inline void setPrecision(std::streamsize precision)
    {
        SetConsoleOutputCP(CP_UTF8);
        std::cout << std::setprecision(precision);
        std::cout << "\033[?25l" << std::flush;
    }
    
    inline void clearConsole()
    {
        std::cout << "\033[2J\033[1;1H";
    }

    inline void setupDedicatedViewport()
    {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

        // \x1b[?1049h -> Switch to Alternate Screen Buffer
        // \x1b[?25l   -> Hide text cursor
        // \x1b[?1003h -> Enable Any-Event Mouse Tracking (Suppresses the hardware arrow cursor) - doesnt seem to work?
        // \x1b[?1006h -> Enable SGR Extended Mouse Reporting coordinates
        std::cout << "\x1b[?1049h\x1b[?25l\x1b[?1003h\x1b[?1006h" << std::flush;
    }

    inline void restoreTerminal()
    {
        // Clean up when your game/renderer closes:
        // \x1b[?25h   -> Show the text cursor again
        // \x1b[?1049l -> Switch back to Main Screen Buffer (restores user's prompt history)
        std::cout << "\x1b[?25h\x1b[?1049l" << std::flush;
    }

    inline void setWindowName(std::wstring name)
    {
        SetConsoleTitleW(name.c_str());
    }

    inline std::pair<size_t, size_t> getCharacterGridSize()
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

        return { static_cast<size_t>(width), static_cast<size_t>(height) };
    }
}