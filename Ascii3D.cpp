#include "ConsoleFunctions.h"
#include "GameEngine.h"
#include <fstream>
#include <iostream>

#include "InputManager.h"
#include <windows.h>

int main()
{
    ConsoleFunctions::setWindowName(L"ASCII Ray-Marcher - Ray Young");
    ConsoleFunctions::setupDedicatedViewport();
    ConsoleFunctions::desyncSTDIO();
    ConsoleFunctions::setPrecision(3);
    ConsoleFunctions::setBackgroundBlack();
    ConsoleFunctions::goFullscreen();
    GameEngine{ 120, 40, 100.0, 0.001 }.run();
    ConsoleFunctions::restoreTerminal();

    return 0;
}
