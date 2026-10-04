#include "GameEngine.h"
#include <execution>
#include <limits>
#include <ranges>

#include "ConsoleFunctions.h"
#include "FileHandler.h"
#include "GameTimer.h"
#include "InputManager.h"
#include "MathFunctions.h"
#include "PropertyModifier.h"
#include "SDFBuilder.h"
#include "SDFOperationInc.h"

GameEngine::GameEngine(size_t width, size_t height, double sceneDepth, double tolerance)
	: m_dimensions{ width, height } , m_sceneDepth{ sceneDepth }, m_surTolerance(tolerance), m_pixelField{ width, height }
    , m_rayMarcher{ sceneDepth, tolerance }, m_camera{ width * .005, height * .01 }
{
    m_framePM = m_perfMonitor.add("FPS", PerfMonitor::FPS);
    m_updatePM = m_perfMonitor.add("Update", PerfMonitor::Micro);
    m_marchPM = m_perfMonitor.add("March");
    m_renderPM = m_perfMonitor.add("Render");
    m_viewportSize = m_camera.setViewportSize({ static_cast<double>(m_dimensions.first), static_cast<double>(m_dimensions.second) });
    InputManager::registerResizeCallback([this](size_t width, size_t height) {
        m_dimensions.first = width;
        m_dimensions.second = height;
        m_pixelField.setSize(m_dimensions.first, m_dimensions.second);
        m_viewportSize = m_camera.setViewportSize({ static_cast<double>(m_dimensions.first), static_cast<double>(m_dimensions.second) });

        if (m_marchMode == Hardware && m_vrm) m_vrm->setCanvasSize(static_cast<uint32_t>(m_dimensions.first), static_cast<uint32_t>(m_dimensions.second));
    });
    GameTimer::reset();
}

void GameEngine::run()
{
    while (true) {
        m_vrm = std::make_unique<VulkanRaymarcher>();

        if (!selectRenderer()) break;

        while (true) {
            if (!selectSceneType()) break;
            if (m_marchMode == Hardware) {
                std::string shaderCode = m_scene.buildComputeShader();
                FileHandler::saveShader(shaderCode, "ShaderSource.glsl");
                m_vrm->setShaderSource(shaderCode);
                m_vrm->setCanvasSize(static_cast<uint32_t>(m_dimensions.first), static_cast<uint32_t>(m_dimensions.second));
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
    if (InputManager::keyJustPushed('M') && InputManager::keyDown(VK_CONTROL)) {
        std::vector<std::string> strVec{};
        m_scene.serialize(strVec);
        FileHandler::saveScene(strVec, "CustomScenes/CustomScene.scn");
    }

    return true;
}

void GameEngine::update()
{
    m_perfMonitor.sample(m_updatePM, [this]() {
        m_camera.update();
        if (m_marchMode == Software) m_scene.update();
    });
}

void GameEngine::march()
{
    m_perfMonitor.sample(m_marchPM, [this]() {
        switch (m_marchMode)
        {
        case GameEngine::Software:
            m_rayMarcher.march(m_scene, m_pixelField, m_camera);
            break;
        case GameEngine::Hardware:
            marchHardware();
            break;
        }
    });
}

bool GameEngine::selectRenderer()
{
    auto& deviceProps = m_vrm->enumerateDevices();
    int selection{ -1 };

    std::function<void()> prompt = [&selection, &deviceProps]() {
        std::cout << "\033[2J\033[1;1H";
        std::cout << "===========================================\n";
        std::cout << "      ASCII Raymarcher 3D - Ray Young\n";
        std::cout << "===========================================\n\n";
        std::cout << "      Rendering Devices\n\n";
        std::cout << "[1] - CPU (Multi-threaded)\n";

        for (size_t i{}; i < deviceProps.size(); ++i) {
            std::cout << "[" << (i + 2) << "] - " << deviceProps[i].deviceName << '\n';
        }
        std::cout << "[" << (deviceProps.size() + 2) << "] - Exit\n";

        std::cout << "\nChoose Your Rendering Device\n\n";
    };

    do prompt(); while (!(std::cin >> selection && selection >= 1 && selection <= 4));    

    if (selection == deviceProps.size() + 2) return false;

    if (selection == 1) {
        m_marchMode = Software;
    } else if (m_vrm->selectDevice(selection - 2)) {
        m_marchMode = Hardware;
    }
    else {
        std::cout << "\n!!! Error configuring selected device. Falling back to Software mode !!!\n";
        m_marchMode = Software;
    }

    return true;
}

bool GameEngine::selectSceneType()
{
    std::cout << "\033[2J\033[1;1H"; // clear console
    std::cout << "Select a scene-type (WARNING: Custom scenes may run very poorly on CPU Renderer)\n\n";
    std::cout << "[1] Built-In (Hardcoded in SDFScene.cpp source file)\n[2] Custom (Defined in the \\CustomScenes folder)\n[3] Back to device selection\n";

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
    int selection{0};
    bool inputFailed{};

    while (true) {
        std::cout << "\033[2J\033[1;1H";
        if (noCustom) std::cout << "No Custom Scenes found\n\n";
        if (inputFailed) std::cout << "- INVALID SELECTION -\n";
        std::cout << "Choose a Built-In Scene\n\n";
        std::cout << "[1] WalkingCubes\n[2] Elongatiions\n[3] Torus\n";
        std::string line;
        if (std::getline(std::cin, line)) {
            std::stringstream ss(line);
            if (ss >> selection) {
                switch (selection) {
                case 1:
                    m_scene = SDFScene{ SDFScene::WalkingCubes };
                    return;
                case 2:
                    m_scene = SDFScene{ SDFScene::Elongate };
                    return;
                case 3:
                    m_scene = SDFScene{ SDFScene::Exclusion };
                    return;
                case 123:
                    m_scene = SDFScene{ SDFScene::Test };
                    return;
                }
            }
        }
        inputFailed = true;
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
                m_scene = SDFScene(scenes[selection - 1]);
                return true;
            }
        }
        inputFailed = true;
    }
}

void GameEngine::marchHardware()
{
    m_vrm->setCanvasSize(static_cast<uint32_t>(m_dimensions.first), static_cast<uint32_t>(m_dimensions.second));
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

    auto result = m_vrm->renderFrame(inputs);
    m_pixelField.fromHardware(result);
}

void GameEngine::render()
{
    m_perfMonitor.sample(m_renderPM, [this]() {
        switch (m_marchMode)
        {
        case GameEngine::Software:
            m_renderer.render(m_pixelField);
            m_pixelField.swap();
            break;
        case GameEngine::Hardware: {
            m_renderer.render(m_pixelField);
            m_pixelField.swap();
        }
            break;
        default:
            break;
        }
    });
}
