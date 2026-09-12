#include "GameEngine.h"
#include <execution>
#include <ranges>
#include "GameTimer.h"
#include "PropertyModifier.h"
#include "SDFOperationInc.h"
#include "MathFunctions.h"

GameEngine::GameEngine(size_t width, size_t height, double sceneDepth, double stepFactor, double tolerance)
	: m_dimensions{ width, height } , m_sceneDepth{ sceneDepth }, m_pixelField{ width, height }
    , m_rayMarcher{ sceneDepth, stepFactor, tolerance }, m_renderer{ width * height }
	, m_camera{ width * .005, height * .01 }
{
    m_scene = SDFScene(SDFScene::LoadOption::Elongate);
    m_framePM = m_perfMonitor.add("FPS", PerfMonitor::FPS);
    m_updatePM = m_perfMonitor.add("Update");
    m_marchPM = m_perfMonitor.add("March");
    m_renderPM = m_perfMonitor.add("Render");
    GameTimer::reset();
}

void GameEngine::run()
{
    while (true) {
        if (handleInput()) break;

        m_perfMonitor.sample(m_framePM, [this]() {
            m_perfMonitor.sample(m_updatePM, [this]() { update(); });
            m_perfMonitor.sample(m_marchPM, [this]() { march(); });
            m_perfMonitor.sample(m_renderPM, [this]() { render(); });
        });
        m_perfMonitor.printMonitors();
    }
}

bool GameEngine::handleInput()
{
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) return true;

    if (GetAsyncKeyState('P') & 0x8000) {
        if (!m_pausePressed) GameTimer::togglePause();
        m_pausePressed = true;
    }
    else {
        m_pausePressed = false;
    }
}

void GameEngine::update()
{
    GameTimer::tick();
    m_camera.update();
    m_scene.update();
}

void GameEngine::march()
{
    m_rayMarcher.march(m_scene, m_pixelField, m_camera);
}

void GameEngine::render()
{
    m_renderer.render(m_pixelField);
    m_pixelField.swap();
}
