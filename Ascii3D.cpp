#include "ConsoleFunctions.h"
#include "GameEngine.h"

int main()
{
    ConsoleFunctions::setupDedicatedViewport();
    ConsoleFunctions::desyncSTDIO();
    ConsoleFunctions::setPrecision(3);
    ConsoleFunctions::goFullscreen();
    GameEngine{ 270, 75, 25.0, 1.0, 0.01 }.run();
    ConsoleFunctions::restoreTerminal();
    return 0;
}
