#include "ConsoleFunctions.h"
#include "GameEngine.h"
#include <fstream>
#include <iostream>

int main()
{
    ConsoleFunctions::setWindowName(L"ASCII Ray-Marcher - Ray Young");
    ConsoleFunctions::setupDedicatedViewport();
    ConsoleFunctions::desyncSTDIO();
    ConsoleFunctions::setPrecision(3);
    ConsoleFunctions::goFullscreen();
    GameEngine{ 15, 5, 100.0, 1.0, 0.001 }.run();
    ConsoleFunctions::restoreTerminal();
    return 0;
}
