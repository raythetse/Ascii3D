#include "GameEngine.h"
#include <execution>
#include <limits>
#include <ranges>

#include "ConsoleFunctions.h"
#include "DemoScenesInc.h"
#include "FileHandler.h"
#include "GameTimer.h"
#include "InputManager.h"
#include "MathFunctions.h"
#include "SDFBuilder.h"
#include "SDFOperationInc.h"

GameEngine::GameEngine(double sceneDepth, double tolerance)
	: m_sceneDepth{ sceneDepth }, m_surTolerance(tolerance), m_rayMarcher{ sceneDepth, tolerance }
{
    m_framePM = m_perfMonitor.add("FPS", PerfMonitor::FPS);
    m_updatePM = m_perfMonitor.add("Update", PerfMonitor::Micro);
    m_marchPM = m_perfMonitor.add("March");
    m_renderPM = m_perfMonitor.add("Render");

    m_dimensions = ConsoleFunctions::getCharacterGridSize();

    m_pixelField.setSize(m_dimensions.first, m_dimensions.second);
    m_viewportSize = m_camera.setViewportSize({ static_cast<double>(m_dimensions.first), static_cast<double>(m_dimensions.second) });

    InputManager::registerResizeCallback([this](size_t width, size_t height) {
        m_dimensions.first = width;
        m_dimensions.second = height;
        m_pixelField.setSize(m_dimensions.first, m_dimensions.second);
        m_viewportSize = m_camera.setViewportSize({ static_cast<double>(m_dimensions.first), static_cast<double>(m_dimensions.second) });

        if (m_marchMode == MARCHINGMODE_HARDWARE && m_vulkanManager) m_vulkanManager->setCanvasSize(static_cast<uint32_t>(m_dimensions.first), static_cast<uint32_t>(m_dimensions.second));
    });
    GameTimer::reset();
}

void GameEngine::run()
{
    while (true) {
        m_vulkanManager = std::make_unique<VulkanManager>();

        if (!selectRenderer()) break;

        while (true) {
            if (!selectSceneType()) break;
            if (m_marchMode == MARCHINGMODE_HARDWARE) {
                std::string shaderCode = m_scene->buildComputeShader();
                FileHandler::saveShader(shaderCode, "ShaderSource.glsl");
                m_vulkanManager->setShaderSource(shaderCode);
                m_vulkanManager->setCanvasSize(static_cast<uint32_t>(m_dimensions.first), static_cast<uint32_t>(m_dimensions.second));
            }
            GameTimer::reset();
            m_camera.reset();
            InputManager::reset();
            while (true) {
                GameTimer::tick();
                if (!handleInput()) break;
                m_perfMonitor.sample(m_framePM, [this]() {
                    update();
                    march();
                    render();
                });
                m_perfMonitor.printMonitors();
            }
        }
    }
}

bool GameEngine::handleInput()
{
    InputManager::update();
    if (InputManager::keyJustPushed(VK_ESCAPE)) return false;
    if (InputManager::keyJustPushed('P')) GameTimer::togglePause();
    if (InputManager::keyDown('R')) GameTimer::reset(GameTimer::isPaused());
    if (InputManager::keyJustPushed(VK_UP)) m_camera.changeMoveSpeed(1.0);
    if (InputManager::keyJustPushed(VK_DOWN)) m_camera.changeMoveSpeed(-1.0);
    if (InputManager::keyJustPushed('T')) m_perfMonitor.toggleMetrics();

    if (InputManager::keyJustPushed('M') && InputManager::keyDown(VK_CONTROL)) {
        std::vector<std::string> strVec{};
        m_scene->serialize(strVec);
        FileHandler::saveScene(strVec, "CustomScenes/CustomScene.scn");
    }

    return true;
}

void GameEngine::update()
{
    m_perfMonitor.sample(m_updatePM, [this]() {
        m_camera.update();
        if (m_marchMode == MARCHINGMODE_SOFTWARE) m_scene->update();
    });
}

void GameEngine::march()
{
    m_perfMonitor.sample(m_marchPM, [this]() {
        switch (m_marchMode)
        {
        case MARCHINGMODE_SOFTWARE:
            m_rayMarcher.march(m_scene.get(), m_pixelField, m_camera);
            break;
        case MARCHINGMODE_HARDWARE:
            marchHardware();
            break;
        }
    });
}

bool GameEngine::selectRenderer()
{
    auto& deviceProps = m_vulkanManager->enumerateDevices();
    int selection{ -1 };

    std::function<void(bool)> prompt = [&selection, &deviceProps](bool controls) {
        std::cout << "\033[2J\033[1;1H";
        if (controls) {
            std::cout << "==============================================================\n";
            std::cout << "[ESCAPE] = Close Scene\n";
            std::cout << "[WASD] = Move Forward/Left/Back/Right\n";
            std::cout << "[SPACE/LSHIFT] = Move Up/Down\n";
            std::cout << "[UP/DOWN] = Increase/Decrease movement speed\n";
            std::cout << "[BACKSPACE] = reset position\n";
            std::cout << "[P] = Pause/Unpause animations\n";
            std::cout << "[R] = Restart animation\n";
            std::cout << "[T] = Toggle performance metrics\n";
            std::cout << "[CTRL M] = Save scene to CustomScene.scn\n";
            std::cout << "[CTRL -/+] = Decrease/Increase font size (affects performance)\n";
            std::cout << "[CTRL 0] = Reset to default font size\n";
            std::cout << "[ALT ENTER] = Toggle fullscreen (affects performance\n";
            std::cout << "==============================================================\n\n";
        }
        else {
            std::cout << "==============================================================\n";
            std::cout << "              ASCII Raymarcher 3D - Ray Young\n";
            std::cout << "==============================================================\n\n";
        }
        std::cout << "      Rendering Devices\n\n";
        std::cout << "[1] - CPU (Slow mode)\n";

        for (size_t i{}; i < deviceProps.size(); ++i) {
            std::cout << "[" << (i + 2) << "] - " << deviceProps[i].deviceName << '\n';
        }
        std::cout << "\n";
        std::cout << "[" << (deviceProps.size() + 2) << "] - Controls and Information\n";
        std::cout << "[" << (deviceProps.size() + 3) << "] - Exit\n";

        std::cout << "\nSelect an option: ";
    };

    do prompt(selection == deviceProps.size() + 2); while (!(std::cin >> selection && selection >= 1 && selection <= deviceProps.size() + 3 && selection != deviceProps.size() + 2));

    if (selection == deviceProps.size() + 3) return false;

    if (selection == 1) {
        m_marchMode = MARCHINGMODE_SOFTWARE;
    } else if (m_vulkanManager->selectDevice(selection - 2)) {
        m_marchMode = MARCHINGMODE_HARDWARE;
    }
    else {
        std::cout << "\n!!! Error configuring selected device. Falling back to Software mode !!!\n";
        m_marchMode = MARCHINGMODE_SOFTWARE;
    }

    return true;
}

bool GameEngine::selectSceneType()
{
    std::cout << "\033[2J\033[1;1H"; // clear console
    std::cout << "Select a scene-type (WARNING: Custom scenes may run very poorly on CPU Renderer)\n\n";
    std::cout << "[1] Built-In (Simple hard-coded demo scenes)\n[2] Custom (Defined in the \\CustomScenes folder)\n[3] Back to device selection\n";

    int selection{};

    while (!(std::cin >> selection && selection >= 1 && selection <= 3)) {
        std::cout << "\033[2J\033[1;1H";
        std::cout << "- INVALID SELECTION -\n[1] Built-in scenes (Hardcoded in SDFScene.cpp source file)\n[2] Custom scenes (Defined in the CustomScenes folder)\n[3] Exit\n";
        std::cin.clear();
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    }

    if (selection == 3) return false;

    bool customFound = false;
    if (selection == 2) {
        if (!selectCustomScene()) {
            selectBuiltInScene(true);
            return true;
        }
    }
    else {
        selectBuiltInScene(false);
    }
    return true;
}

void GameEngine::selectBuiltInScene(bool noCustom)
{
    int selection{};

    while (true) {
        std::cout << "\033[2J\033[1;1H";
        if (noCustom) std::cout << "No Custom Scenes found\n\n";
        std::cout << "Choose a Built-In Scene\n\n";
        std::cout << "[1] Shapes - Collection of Raymarched shapes.\n";
        std::cout << "[2] Combination - Add, subtract, intersect.  Smoothing in the back row.\n";
        std::cout << "[3] Walking Cubes - Classic cube rolling.\n";
        std::cout << "[4] Exclusion - Removing overlapping spaces, coloring interior spaces.\n";
        std::cout << "[5] Worms - Combining partial toruses into continuous moving worms.\n";
        std::cout << "[6] Rainbow - Cycling colors across multiple objects.\n";
        std::cout << "[7] Jigsaw - Domain Repetition tricks.\n";
        std::cout << "[8] Revolution - Dynamic animation of a star-shaped ring.\n";
        std::cout << "[9] Color Blend - Blending colors in varying fashion.\n";
        std::cout << "[10] Elongation - Elongating a sphere on three Axes.\n";
        std::cout << "[11] Smooth Exclude - Germophobic spheres? Avoid direct contact.\n";
        std::string line;
        if (std::getline(std::cin, line)) {
            std::stringstream ss(line);
            if (ss >> selection) {
                switch (selection) {
                case 1:
                    m_scene = std::make_unique<ShapesScene>();
                    //m_scene = std::make_unique<TestScene>();
                    return;
                case 2:
                    m_scene = std::make_unique<CombinationScene>();
                    return;
                case 3:
                    m_scene = std::make_unique<WalkingCubesScene>();
                    return;
                case 4:
                    m_scene = std::make_unique<ExclusionScene>();
                    return;
                case 5:
                    m_scene = std::make_unique<WormsScene>();
                    return;
                case 6:
                    m_scene = std::make_unique<RainbowScene>();
                    return;
                case 7:
                    m_scene = std::make_unique<JigsawScene>();
                    return;
                case 8:
                    m_scene = std::make_unique<RevolutionScene>();
                    return;
                case 9:
                    m_scene = std::make_unique<ColorBlendScene>();
                    return;
                case 10:
                    m_scene = std::make_unique<ElongateScene>();
                    return;
                case 11:
                    m_scene = std::make_unique<ExcludeSmoothScene>();
                    return;
                }
            }
        }
    }
}

bool GameEngine::selectCustomScene()
{
    std::vector<std::string> scenes{};
    FileHandler::enumerateCustomScenes(scenes);

    if (!scenes.size()) return false;

    bool inputFailed{};
    int selection{}; 
    std::string line;
    std::cin.clear();
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    while (true) {
        std::cout << "\033[2J\033[1;1H";
        if (inputFailed) std::cout << "- INVALID SELECTION -\n\n";
        std::cout << "Select a Custom Scene\n\n";
        for (size_t i{}; i < scenes.size(); ++i) {
            std::cout << "[" << (i + 1) << "] " << scenes[i] << '\n';
        }

        if (std::getline(std::cin, line)) {
            std::stringstream ss(line);
            if (ss >> selection && selection >= 1 && selection <= scenes.size()) {
                m_scene = std::make_unique<SDFScene>(scenes[selection - 1]);
                if (confirmLoadWarnings(scenes[selection - 1])) return true;
                m_scene.reset();
                inputFailed = false;
                continue;
            }
        }
        inputFailed = true;
    }
}

// The loader recovers from malformed lines, but the problems found (validator findings,
// else loader warnings; see SDFScene) would be overwritten by the first rendered frame,
// so show them here before rendering starts.
// Returns true to render the scene, false to go back to the scene list.
bool GameEngine::confirmLoadWarnings(const std::string& sceneName)
{
    const auto& warnings = m_scene->loadWarnings();
    if (warnings.empty()) return true;

    // Leave room for the header and the prompt so the list fits on screen.
    const size_t maxShown = m_dimensions.second > 9 ? m_dimensions.second - 9 : 1;

    std::cout << "\033[2J\033[1;1H";
    std::cout << sceneName << " has " << warnings.size()
              << (warnings.size() == 1 ? " problem" : " problems")
              << ". The loader skips the affected lines or uses defaults.\n"
              << "Rule numbers refer to ClaudeNotes/scene_format_spec_v0.4.md, section 11.\n\n";
    for (size_t i{}; i < warnings.size() && i < maxShown; ++i) {
        std::cout << "  " << warnings[i] << '\n';
    }
    if (warnings.size() > maxShown) {
        std::cout << "  ... and " << (warnings.size() - maxShown) << " more\n";
    }
    std::cout << "\n[ENTER] = Render anyway\n[B] = Back to scene list\n" << std::flush;

    std::string line;
    if (!std::getline(std::cin, line)) return true;
    return !(line == "b" || line == "B");
}

void GameEngine::marchHardware()
{
    m_vulkanManager->setCanvasSize(static_cast<uint32_t>(m_dimensions.first), static_cast<uint32_t>(m_dimensions.second));
    Vec3 camPos = m_camera.getPosition();
    Vec3 camForward = m_camera.getForward();
    Vec3 camRight = m_camera.getRight();
    Vec3 camUp = m_camera.getUp();

    ShaderInputs inputs{};

    inputs.time             = static_cast<float>(GameTimer::gameTime());
    inputs.width            = static_cast<float>(m_dimensions.first);
    inputs.height           = static_cast<float>(m_dimensions.second);
    inputs.sceneDepth       = static_cast<float>(m_sceneDepth);
    inputs.surfTolerance    = static_cast<float>(m_surTolerance);

    inputs.camPos[0]        = static_cast<float>(camPos.x);
    inputs.camPos[1]        = static_cast<float>(camPos.y);
    inputs.camPos[2]        = static_cast<float>(camPos.z);

    inputs.camForward[0]    = static_cast<float>(camForward.x);
    inputs.camForward[1]    = static_cast<float>(camForward.y);
    inputs.camForward[2]    = static_cast<float>(camForward.z);

    inputs.camRight[0]      = static_cast<float>(camRight.x);
    inputs.camRight[1]      = static_cast<float>(camRight.y);
    inputs.camRight[2]      = static_cast<float>(camRight.z);

    inputs.camUp[0]         = static_cast<float>(camUp.x);
    inputs.camUp[1]         = static_cast<float>(camUp.y);
    inputs.camUp[2]         = static_cast<float>(camUp.z);

    inputs.viewportSize[0]  = static_cast<float>(m_viewportSize.x);
    inputs.viewportSize[1]  = static_cast<float>(m_viewportSize.y);

    auto result = m_vulkanManager->renderFrame(inputs);
    m_pixelField.fromHardware(result);
}

void GameEngine::render()
{
    m_perfMonitor.sample(m_renderPM, [this]() {
        m_renderer.render(m_pixelField);
        m_pixelField.swap();
    });
}
