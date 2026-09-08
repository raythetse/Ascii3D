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
        SOBuilder().pushOp<SDFOp::Add,  SDF::BoxFrame>(Vec3::Up, Vec3::Zero, Vec3::One * 1.25, 0.1, Color::White)()
        .pushOp<SDFOp::AddSmooth,       SDF::Box>(Vec3{ -0.5, -0.5, -0.5 },     Vec3::Zero, Vec3::One, Color::MediumGray)(0.5, 5.0)
        .pushOp<SDFOp::Add,             SDF::Box>(Vec3{ 0.5, -0.5, -0.5 },      Vec3::Zero, Vec3::One, Color::Red)()
        .pushOp<SDFOp::RoundOff>()
        .pushOp<SDFOp::Add,             SDF::Box>(Vec3{ -0.5, -0.5, 0.5 },      Vec3::Zero, Vec3::One, Color::Orange)()
        .pushOp<SDFOp::RoundOff>()
        .pushOp<SDFOp::Add,             SDF::Box>(Vec3{ 0.5, -0.5, 0.5 },       Vec3::Zero, Vec3::One, Color::Yellow)()
        .pushOp<SDFOp::Add,             SDF::Box>(Vec3{ 0.5, 0.5, 0.5 },        Vec3::Zero, Vec3::One, Color::Green)()
        .pushOp<SDFOp::RoundOff>()
        .pushOp<SDFOp::Add,             SDF::Box>(Vec3{ -0.5, 0.5, 0.5 },       Vec3::Zero, Vec3::One, Color::Blue) ()
        .pushOp<SDFOp::Add,             SDF::Box>(Vec3{ 0.5, 0.5, -0.5 },       Vec3::Zero, Vec3::One, Color::Violet)()
        .pushOp<SDFOp::Add,             SDF::Box>(Vec3{ -0.5, 0.5, -0.5 },      Vec3::Zero, Vec3::One, Color::Magenta)()
        .pushOp<SDFOp::RoundOff>()
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
