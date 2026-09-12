#include "ConsoleFunctions.h"
#include "GameEngine.h"

int main()
{
    ConsoleFunctions::desyncSDTIO();
    ConsoleFunctions::setPrecision(3);
    ConsoleFunctions::goFullscreen();
    GameEngine{ 270, 75, 25.0, 1.0, 0.01 }.run();
    return 0;
}
