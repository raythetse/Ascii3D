#include "GameEngine.h"
#include <execution>
#include <ranges>

#include "ConsoleFunctions.h"
#include "FileHandler.h"
#include "GameTimer.h"
#include "InputManager.h"
#include "MathFunctions.h"
#include "PropertyModifier.h"
#include "SDFOperationInc.h"
#include "SDFBuilder.h"

GameEngine::GameEngine(size_t width, size_t height, double sceneDepth, double tolerance)
	: m_dimensions{ width, height } , m_sceneDepth{ sceneDepth }, m_surTolerance(tolerance), m_pixelField{ width, height }
    , m_rayMarcher{ sceneDepth, tolerance }, m_camera{ width * .005, height * .01 }
{
    m_framePM = m_perfMonitor.add("FPS", PerfMonitor::FPS);
    m_updatePM = m_perfMonitor.add("Update", PerfMonitor::Micro);
    m_marchPM = m_perfMonitor.add("March");
    m_renderPM = m_perfMonitor.add("Render");
    InputManager::registerResizeCallback([this](size_t width, size_t height) {
        m_dimensions.first = width;
        m_dimensions.second = height;
        m_pixelField.setSize(m_dimensions.first, m_dimensions.second);
        m_viewportSize = m_camera.setViewportSize({ static_cast<double>(m_dimensions.first), static_cast<double>(m_dimensions.second) });

        if (m_marchMode == Hardware) m_vrm.setCanvasSize(static_cast<uint32_t>(m_dimensions.first), static_cast<uint32_t>(m_dimensions.second));
    });
    GameTimer::reset();
}

void GameEngine::run()
{
    m_scene = SDFScene(SDFScene::LoadOption::Test);
    //m_scene = SDFScene("MyFile.txt");

    m_marchMode = m_vrm.selectDevice() ? Hardware : Software;
    if (m_marchMode == Hardware) {
        std::string shaderCode = m_scene.buildComputeShader();
        FileHandler::saveToFile(shaderCode, "ShaderSource.glsl");
        m_vrm.setShaderSource(shaderCode);
        m_vrm.setCanvasSize(static_cast<uint32_t>(m_dimensions.first), static_cast<uint32_t>(m_dimensions.second));
    }

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

bool GameEngine::handleInput()
{
    InputManager::update();
    if (InputManager::keyJustPushed(VK_ESCAPE)) return false;
    if (InputManager::keyJustPushed('P')) GameTimer::togglePause();
    if (InputManager::keyJustPushed('M')) {
        std::vector<std::string> strVec{};
        m_scene.serialize(strVec);
        FileHandler::saveToFile(strVec, "MyFile.txt");
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

void GameEngine::marchHardware()
{
    m_vrm.setCanvasSize(static_cast<uint32_t>(m_dimensions.first), static_cast<uint32_t>(m_dimensions.second));
    Vec3 camPos = m_camera.getPosition();
    Vec3 camForward = m_camera.getForward();
    Vec3 camRight = m_camera.getRight();
    Vec3 camUp = m_camera.getUp();

    ShaderInputs inputs{};

    inputs.width = static_cast<float>(m_dimensions.first);
    inputs.height = static_cast<float>(m_dimensions.second);
    inputs.time = static_cast<float>(GameTimer::gameTime());
    inputs.sceneDepth = static_cast<float>(m_sceneDepth);
    inputs.surfTolerance = static_cast<float>(m_surTolerance);

    inputs.camPos[0] = static_cast<float>(camPos.x);
    inputs.camPos[1] = static_cast<float>(camPos.y);
    inputs.camPos[2] = static_cast<float>(camPos.z);

    inputs.camForward[0] = static_cast<float>(camForward.x);
    inputs.camForward[1] = static_cast<float>(camForward.y);
    inputs.camForward[2] = static_cast<float>(camForward.z);

    inputs.camRight[0] = static_cast<float>(camRight.x);
    inputs.camRight[1] = static_cast<float>(camRight.y);
    inputs.camRight[2] = static_cast<float>(camRight.z);

    inputs.camUp[0] = static_cast<float>(camUp.x);
    inputs.camUp[1] = static_cast<float>(camUp.y);
    inputs.camUp[2] = static_cast<float>(camUp.z);
    inputs.viewportSize[0] = m_viewportSize.x;
    inputs.viewportSize[1] = m_viewportSize.y;

    auto result = m_vrm.renderFrame(inputs);
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
