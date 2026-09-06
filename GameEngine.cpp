#include "GameEngine.h"
#include <execution>
#include <ranges>
#include "Operation.h"

GameEngine::GameEngine(size_t width, size_t height, double sceneDepth, double tolerance)
	: m_dimensions{ width, height }
	, m_sceneDepth{ sceneDepth }
	, m_pixelField{ width, height }
	, m_rayMarcher{ sceneDepth, tolerance }
	, m_renderer{ width * height }
	, m_camera{ width * .005, height * .01 }
    , m_pixelRange{ std::views::iota(size_t(0), width * height) }
    , m_objectRange{ std::views::iota(size_t(0), width * height) }
{
    // FLOOR PLANE
    auto ground = std::make_unique<SceneObject>();
    auto groundBox = std::make_unique<Box>(Vec3{ 0.0, 0.0, 10.0 }, Vec3{ 0.0, 0.0, 0.0 }, Vec3{ 25.0, 0.0, 25.0 }, Color::LightGray);
    ground->pushOperation(std::make_unique<SDFOp::Add>(std::move(groundBox)));
    //m_objects.push_back(std::move(ground));

    // OBJECTS
    auto baseObj = std::make_unique<SceneObject>();
    auto obj1 = std::make_unique<Sphere>(Vec3{ 2.0, 1.0, 5.0 }, 1.0, Color::Red);
    auto obj2 = std::make_unique<Sphere>(Vec3{ 1.0, 2.0, 5.0 }, 1.0, Color::Blue);

    //obj1->addOperation(std::move(std::make_unique<SDFOp::Onion>(0.4)));
    //obj1->addOperation(std::move(std::make_unique<SDFOp::Onion>(0.2)));
    //obj1->addOperation(std::move(std::make_unique<SDFOp::Onion>(0.1)));
    //obj1->addOperation(std::move(std::make_unique<SDFOp::Subtract>(std::move(obj2))));
    baseObj->pushOperation(std::move(std::make_unique<SDFOp::MirrorX>()));
    baseObj->pushOperation(std::move(std::make_unique<SDFOp::Add>(std::move(obj1))));
    baseObj->pushOperation(std::move(std::make_unique<SDFOp::Add>(std::move(obj2))));

    m_objects.push_back(std::move(baseObj));
    //m_objects.push_back(std::move(obj2));

    m_lights.push_back(std::make_unique<LightSource>(Vec3{ 0.0, 5.0, 5.0 }, 0.25));
    //m_lights.push_back(std::make_unique<LightSource>(Vec3{ -4.0, 4.0, 8.0 }, 0.15));

    m_objectRange = std::views::iota(size_t(0), m_objects.size());

    m_camera.reset();
}

void GameEngine::update()
{
    m_deltaTime = m_gameTimer.tick(Stopwatch::Second);
    m_time = m_gameTimer.totalTime(Stopwatch::Second);
    m_camera.update(m_deltaTime);

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
