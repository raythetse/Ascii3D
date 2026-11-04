#include "ConsoleFunctions.h"
#include "GameEngine.h"
#include <fstream>
#include <iostream>
#include <span>
#include <sstream>
#include <string_view>

#include "InputManager.h"
#include "SceneValidator.h"
#include <windows.h>

// Ascii3D.exe --validate FILE...
// Prints every validator finding as "path:line: error|warning: [rule N] message".
// Returns 1 if any file has an error or cannot be read, 0 otherwise.
static int validateFiles(std::span<char*> paths)
{
    if (paths.empty()) {
        std::cerr << "usage: Ascii3D.exe --validate FILE...\n";
        return 2;
    }
    bool anyError{};
    for (const char* path : paths) {
        std::ifstream in{ path, std::ios::binary };
        if (!in) {
            std::cout << path << ": error: cannot read file\n";
            anyError = true;
            continue;
        }
        std::stringstream text;
        text << in.rdbuf();
        for (const auto& finding : SceneValidator::validate(text.str())) {
            std::cout << finding.format(path) << '\n';
            anyError |= finding.severity == SceneValidator::Severity::Error;
        }
    }
    return anyError ? 1 : 0;
}

int main(int argc, char* argv[])
{
    // Command-line mode: no console setup, no window, no Vulkan
    if (argc > 1 && std::string_view{ argv[1] } == "--validate") {
        return validateFiles(std::span{ argv + 2, static_cast<size_t>(argc - 2) });
    }

    ConsoleFunctions::setWindowName(L"ASCII Ray-Marcher - Ray Young");
    ConsoleFunctions::setupDedicatedViewport();
    ConsoleFunctions::desyncSTDIO();
    ConsoleFunctions::setPrecision(3);
    ConsoleFunctions::setBackgroundBlack();
    ConsoleFunctions::DisableConsoleQuickEdit();
    SetProcessDPIAware();
    //ConsoleFunctions::goFullscreen();
    GameEngine{ 100.0, 0.001 }.run();
    ConsoleFunctions::restoreTerminal();

    return 0;
}
