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

GameEngine::GameEngine(size_t width, size_t height, double sceneDepth, double stepFactor, double tolerance)
	: m_dimensions{ width, height } , m_sceneDepth{ sceneDepth }, m_pixelField{ width, height }
    , m_rayMarcher{ sceneDepth, stepFactor, tolerance }, m_camera{ width * .005, height * .01 }
{
    m_framePM = m_perfMonitor.add("FPS", PerfMonitor::FPS);
    m_updatePM = m_perfMonitor.add("Update", PerfMonitor::Micro);
    m_marchPM = m_perfMonitor.add("March");
    m_renderPM = m_perfMonitor.add("Render");
    InputManager::registerResizeCallback([this](size_t width, size_t height) {
        m_pixelField.setSize(width, height);
        m_camera.setViewportSize({ static_cast<double>(width), static_cast<double>(height) });
    });
    GameTimer::reset();
}

void GameEngine::run()
{
    //m_scene = SDFScene(SDFScene::LoadOption::Test);
    std::vector<std::string> strVec;
    FileHandler::readSDFScene(strVec, "MyFile.txt");

    SDFBuilder builder{};
    builder.loadSerialized(strVec);

    m_scene = SDFScene(builder);

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
        FileHandler::saveSDFScene(strVec, "MyFile.txt");
    }

    return true;
}

void GameEngine::update()
{
    m_perfMonitor.sample(m_updatePM, [this]() {
        m_camera.update();
        m_scene.update();
    });
}

void GameEngine::march()
{
    m_perfMonitor.sample(m_marchPM, [this]() {
        m_rayMarcher.march(m_scene, m_pixelField, m_camera);
    });
}

void GameEngine::render()
{
    m_perfMonitor.sample(m_renderPM, [this]() {
        m_renderer.render(m_pixelField);
        m_pixelField.swap();
    });
}
