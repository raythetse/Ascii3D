#include "ConsoleFunctions.h"
#include "GameEngine.h"
#include <fstream>
#include <iostream>

#include "InputManager.h"
#include "VulkanRaymarcher.h"
#include <windows.h>

int main()
{
    ConsoleFunctions::setWindowName(L"ASCII Ray-Marcher - Ray Young");
    ConsoleFunctions::setupDedicatedViewport();
    ConsoleFunctions::desyncSTDIO();
    ConsoleFunctions::setPrecision(3);
    size_t cWidth{ 120 };
    size_t cHeight{ 40 };

    VulkanRaymarcher vrm{};
    vrm.init();

    InputManager::registerResizeCallback([&cWidth, &cHeight, &vrm](size_t width, size_t height) {
        cWidth = width;
        cHeight = height;
        vrm.resizeCanvas(cWidth, cHeight);
        std::cout << "\nResized... Width: " << cWidth << " - Height: " << cHeight << '\n';
    });
    ConsoleFunctions::goFullscreen();
    //GameEngine{ 15, 5, 100.0, 1.0, 0.001 }.run();
    //ConsoleFunctions::restoreTerminal();

    std::cout << "\nWidth: " << cWidth << " - Height: " << cHeight << '\n';

    vrm.resizeCanvas(cWidth, cHeight);

    RaymarchPushConstants inputs{};
    inputs.customFloat = 3.14;

    const std::string charGradient{ "-~+*vri325E8B@" };
    while (true) {
        InputManager::update();
        inputs.width = cWidth;
        inputs.height = cHeight;
        auto pixelOutput = vrm.renderFrame(inputs);
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD coord = { 0, 0 };
        SetConsoleCursorPosition(hConsole, coord);
        std::string buffer{};
        for (size_t i{}; i < cHeight; ++i) {
            std::cout << '\n';
            for (size_t j{}; j < cWidth; ++j) {
                size_t pixelIndex = static_cast<size_t>(i * cWidth + j);
                int index = std::floor(pixelOutput[i * cWidth + j].r * pixelOutput[i * cWidth + j].g * (charGradient.size() - 1));
                buffer += charGradient[index];
            }
        }
        std::cout << buffer;
    }
    std::string waitInput;
    std::cin >> waitInput;

    return 0;
}
