#include <iostream>
#include <thread>
#include <windows.h>
#include "PerfMonitor.h"
#include "Stopwatch.h"

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

int main()
{
    setupConsole();
    double time{};
    GameEngine gEngine{ 270, 75, 25.0, 0.01 };
    //GameEngine gEngine{ 200, 55, 15.0, 0.01 };
    PerfMonitor pMonitor{};
    auto framePM = pMonitor.add("FPS", PerfMonitor::FPS);
    auto updatePM = pMonitor.add("Update", PerfMonitor::Micro);
    auto marchPM = pMonitor.add("March", PerfMonitor::Milli);
    auto renderPM = pMonitor.add("Render");
    Stopwatch sw{};

    sw.reset();
    while (true) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;
        pMonitor.sample(framePM, [&gEngine, &pMonitor, &updatePM, &marchPM, &renderPM, &time, &sw]() {
            pMonitor.sample(updatePM, [&gEngine]() { gEngine.update(); });
            pMonitor.sample(marchPM, [&gEngine]() { gEngine.march(); });
            pMonitor.sample(renderPM, [&gEngine]() { gEngine.render(); });
        });
        pMonitor.printMonitors();
    }

    return 0;
}
