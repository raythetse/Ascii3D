#include <iostream>
#include <thread>
#include <windows.h>
#include "PerfMonitor.h"
#include "Stopwatch.h"
#include "GameTimer.h"

#include "GameEngine.h"

void setupConsole() {
    // key events to go full screen
    keybd_event(VK_MENU, 0x38, 0, 0);
    keybd_event(VK_RETURN, 0x1c, 0, 0);
    keybd_event(VK_RETURN, 0x1c, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);

    // turn off stdio sync for performance
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    SetConsoleOutputCP(CP_UTF8);
    std::cout << std::setprecision(3);
    std::cout << "\033[?25l" << std::flush;
}

void closeConsole() {
    HWND myConsole = GetConsoleWindow();
    PostMessage(myConsole, WM_CLOSE, 0, 0);
}

int main()
{
    setupConsole();
    double time{};
    GameEngine gEngine{ 270, 75, 25.0, 1.0, 0.005 };
    PerfMonitor pMonitor{};
    auto framePM = pMonitor.add("FPS", PerfMonitor::FPS);
    auto updatePM = pMonitor.add("Update");
    auto renderPM = pMonitor.add("Render");
    Stopwatch sw{};
    bool pausePressed{};

    sw.reset();
    while (true) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;
        if (GetAsyncKeyState('P') & 0x8000) {
            if (!pausePressed) GameTimer::togglePause();
            pausePressed = true;
        }
        else {
            pausePressed = false;
        }
        pMonitor.sample(framePM, [&gEngine, &pMonitor, &updatePM, &renderPM, &time, &sw]() {
            pMonitor.sample(updatePM, [&gEngine]() { gEngine.update(); });
            pMonitor.sample(renderPM, [&gEngine]() { gEngine.render(); });
        });
        pMonitor.printMonitors();
    }

    return 0;
}
