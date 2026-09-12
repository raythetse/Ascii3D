#include "ConsoleFunctions.h"
#include "GameEngine.h"

int main()
{
    ConsoleFunctions::desyncSTDIO();
    ConsoleFunctions::setPrecision(3);
    ConsoleFunctions::goFullscreen();
    GameEngine{ 80, 25, 25.0, 1.0, 0.01 }.run();
    return 0;
}
