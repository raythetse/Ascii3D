#include "GameEngine.h"
#include <execution>
#include <ranges>
#include "Operation.h"

GameEngine::GameEngine(size_t width, size_t height, double sceneDepth, double stepFactor, double tolerance)
	: m_dimensions{ width, height }
	, m_sceneDepth{ sceneDepth }
	, m_pixelField{ width, height }
	, m_rayMarcher{ sceneDepth, stepFactor, tolerance }
	, m_renderer{ width * height }
	, m_camera{ width * .005, height * .01 }
    , m_pixelRange{ std::views::iota(size_t(0), width * height) }
    , m_objectRange{ std::views::iota(size_t(0), width * height) }
{
    auto floor = std::make_unique<SceneObject>();
    auto box = std::make_unique<Box>(Vec3::Down, Vec3::Zero, Vec3{ 10.0, 0.0, 10.0 });
    floor->pushOperation(std::move(std::make_unique<SDFOp::Add>(std::move(box))));
    m_objects.push_back(std::move(floor));

    auto baseObj = std::make_unique<SceneObject>();
    auto obj1 = std::make_unique<Box>(Vec3{ 0.5, 0.0, 0.0 }, Vec3::Right * 0.125, Vec3::One, Color::LightGreen);
    auto obj2 = std::make_unique<Box>(Vec3{ -0.5, 0.0, 0.0 }, Vec3{ 1.0, -1.0, 0.0 }.getNormal() * 0.25, Vec3::One, Color::LightBlue);
    auto obj3 = std::make_unique<Box>(Vec3{ 0.0, 1.0, 0.0 }, Vec3{ -1.0, -1.0, 0.0 }.getNormal() * 0.25, Vec3::One, Color::Pink);

    baseObj->pushOperation(std::move(std::make_unique<SDFOp::AddSmooth>(std::move(obj1), 0.5, 10.0)));
    //baseObj->pushOperation(std::move(std::make_unique<SDFOp::AddSmooth>(std::move(obj2), 0.5, 10.0)));
    //baseObj->pushOperation(std::move(std::make_unique<SDFOp::AddSmooth>(std::move(obj3), 0.5, 10.0)));
    baseObj->pushOperation(std::move(std::make_unique<SDFOp::RoundOff>(0.1)));

    m_objects.push_back(std::move(baseObj));

    m_lights.push_back(std::make_unique<LightSource>(Vec3{ 0.0, 3.0, -4.0 }, 0.25));

    m_objectRange = std::views::iota(size_t(0), m_objects.size());

    m_camera.reset();
}

void GameEngine::update()
{
    m_deltaTime = m_gameTimer.tick(Stopwatch::Second);
    m_camera.update(m_deltaTime);

    m_time = m_gameTimer.totalTime(Stopwatch::Second);
    for (auto& object : m_objects) object->update(m_time);
    for (auto& light : m_lights) light->update(m_time);
}

void GameEngine::march()
{
    m_rayMarcher.march(m_pixelField, m_objects, m_lights, m_camera, m_pixelRange, m_time);
}

void GameEngine::render()
{
    m_renderer.render(m_pixelField);
    m_pixelField.swap();
}
