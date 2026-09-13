#include "GameEngine.h"
#include <execution>
#include <ranges>

#include "ConsoleFunctions.h"
#include "GameTimer.h"
#include "InputManager.h"
#include "MathFunctions.h"
#include "PropertyModifier.h"
#include "SDFOperationInc.h"

GameEngine::GameEngine(size_t width, size_t height, double sceneDepth, double stepFactor, double tolerance)
	: m_dimensions{ width, height } , m_sceneDepth{ sceneDepth }, m_pixelField{ width, height }
    , m_rayMarcher{ sceneDepth, stepFactor, tolerance }, m_renderer{ width * height }
	, m_camera{ width * .005, height * .01 }
{
    m_scene = SDFScene(SDFScene::Elongate);
    m_framePM = m_perfMonitor.add("FPS", PerfMonitor::FPS);
    m_updatePM = m_perfMonitor.add("Update", PerfMonitor::Micro);
    m_marchPM = m_perfMonitor.add("March");
    m_renderPM = m_perfMonitor.add("Render");
    GameTimer::reset();
}

void GameEngine::run()
{
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

    if      (InputManager::keyJustPushed(VK_UP))    m_camera.setViewportSize(m_pixelField.decreaseHeight(InputManager::keyDown(VK_CONTROL) ? 5  : 1));
    else if (InputManager::keyJustPushed(VK_DOWN))  m_camera.setViewportSize(m_pixelField.increaseHeight(InputManager::keyDown(VK_CONTROL) ? 5  : 1));
    else if (InputManager::keyJustPushed(VK_LEFT))  m_camera.setViewportSize(m_pixelField.decreaseWidth (InputManager::keyDown(VK_CONTROL) ? 15 : 3));
    else if (InputManager::keyJustPushed(VK_RIGHT)) m_camera.setViewportSize(m_pixelField.increaseWidth (InputManager::keyDown(VK_CONTROL) ? 15 : 3));

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
