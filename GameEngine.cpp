#include "GameEngine.h"
#include <execution>
#include <ranges>
#include "Operation.h"
#include "SceneObjectBuilder.h"

void GameEngine::add(SOBuilder& obj)
{
    m_objects.push_back(obj.get());
}

GameEngine::GameEngine(size_t width, size_t height, double sceneDepth, double stepFactor, double tolerance)
	: m_dimensions{ width, height } , m_sceneDepth{ sceneDepth }, m_pixelField{ width, height }, m_rayMarcher{ sceneDepth, stepFactor, tolerance }, m_renderer{ width * height }
	, m_camera{ width * .005, height * .01 }, m_pixelRange{ std::views::iota(size_t(0), width * height) }, m_objectRange{ std::views::iota(size_t(0), width * height) }
{
    //add(SOBuilder().pushOp<SDFOp::Add, PlaneXZ>()());

    add(
        SOBuilder().pushOp<SDFOp::Add, Box>(Vec3::Up, Vec3::Zero, Vec3::One, Color::Red)()
            .pushOp<SDFOp::AddSmooth, Box>(Vec3{ 0.5 }, Vec3::Zero, Vec3::One, Color::Green)()
                //.back()
                .pushOp<SDFOp::Add, Box>(Vec3{ -0.5, 0.5, 0.5 }, Vec3::One.getNormal() * 0.125, Vec3::One, Color::Blue)()
    );

    m_lights.push_back(std::make_unique<LightSource>((Vec3::Up + Vec3::Right + Vec3::Backward) * 4.0, 0.5));

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
