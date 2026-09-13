#include "ConsoleFunctions.h"
#include "GameEngine.h"

int main()
{
    ConsoleFunctions::setWindowName(L"3D ASCII Ray-Marcher - by Ray Young");
    ConsoleFunctions::setupDedicatedViewport();
    ConsoleFunctions::desyncSTDIO();
    ConsoleFunctions::setPrecision(3);
    ConsoleFunctions::goFullscreen();
    GameEngine{ 15, 5, 25.0, 1.0, 0.01 }.run();
    ConsoleFunctions::restoreTerminal();
    return 0;
}
