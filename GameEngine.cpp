#include "GameEngine.h"
#include <execution>
#include <ranges>
#include "GameTimer.h"
#include "Operation.h"
#include "SceneObjectBuilder.h"

void GameEngine::add(SOBuilder& obj)
{
    m_objects.push_back(obj.get());
}

void GameEngine::add(SOBuilder&& obj)
{
    m_objects.push_back(obj.get());
}

GameEngine::GameEngine(size_t width, size_t height, double sceneDepth, double stepFactor, double tolerance)
	: m_dimensions{ width, height } , m_sceneDepth{ sceneDepth }, m_pixelField{ width, height }, m_rayMarcher{ sceneDepth, stepFactor, tolerance }, m_renderer{ width * height }
	, m_camera{ width * .005, height * .01 }, m_pixelRange{ std::views::iota(size_t(0), width * height) }, m_objectRange{ std::views::iota(size_t(0), width * height) }
{
    //buildCubes();
    //buildAtom();
    buildMotionDemo();
    

    m_objectRange = std::views::iota(size_t(0), m_objects.size());
    GameTimer::reset();
}

void GameEngine::update()
{
    GameTimer::tick();
    m_camera.update();
    m_rayMarcher.march(m_pixelField, m_objects, m_lights, m_camera, m_pixelRange);
}

void GameEngine::render()
{
    m_renderer.render(m_pixelField);
    m_pixelField.swap();
}

void GameEngine::buildCubesDemo()
{
    add(SOBuilder{}.pushOp<SDFOp::Add, SDF::Box>(Vec3::Down * 3.0, Vec3::Zero, Vec3{ 25.0, 0.0, 25.0 }, Color::SkyBlue)());
    add(SOBuilder{}
        .pushOp<SDFOp::RotateActive>(Vec3::Up, 0.1)
        .pushOp<SDFOp::RotateActive>(Vec3::Right, 0.1)
        .pushOp<SDFOp::RotateActive>(Vec3::Forward, 0.1)
        .pushOp<SDFOp::Add, SDF::BoxFrame>(Vec3::Up, Vec3::Zero, Vec3{ 1.75 }, 0.1, Color::White)().back()
        .pushOp<SDFOp::TranslateSine>(Vec3::Up, 2.0)
        .pushOp<SDFOp::RotateActive>(Vec3::Down, 0.1)
        .pushOp<SDFOp::RotateActive>(Vec3::Left, 0.1)
        .pushOp<SDFOp::RotateActive>(Vec3::Backward, 0.1)
        .pushOp<SDFOp::Add, SDF::Box>(Vec3{ 0.5, -0.5, -0.5 }, Vec3::Zero, Vec3::One, Color::Red)()
        .pushOp<SDFOp::Add, SDF::Box>(Vec3{ -0.5, -0.5, 0.5 }, Vec3::Zero, Vec3::One, Color::Orange)()
        .pushOp<SDFOp::Add, SDF::Box>(Vec3{ -0.5, 0.5, -0.5 }, Vec3::Zero, Vec3::One, Color::Magenta)()
        .pushOp<SDFOp::Add, SDF::Box>(Vec3{ 0.5, 0.5, 0.5 }, Vec3::Zero, Vec3::One, Color::MediumGray)().back(3).pushOp<SDFOp::RoundOff>(0.1)
        .pushOp<SDFOp::Add, SDF::Box>(Vec3{ -0.5, -0.5, -0.5 }, Vec3::Zero, Vec3::One, Color::Green)()
        .pushOp<SDFOp::Add, SDF::Box>(Vec3{ 0.5, -0.5, 0.5 }, Vec3::Zero, Vec3::One, Color::Yellow)()
        .pushOp<SDFOp::Add, SDF::Box>(Vec3{ -0.5, 0.5, 0.5 }, Vec3::Zero, Vec3::One, Color::Blue)()
        .pushOp<SDFOp::Add, SDF::Box>(Vec3{ 0.5, 0.5, -0.5 }, Vec3::Zero, Vec3::One, Color::Violet)()
    );
    m_lights.push_back(std::make_unique<LightSource>(Vec3{ 1.0, 4.0, -1.0 }, 1.0));
}

void GameEngine::buildAtomDemo()
{
    auto builder = SOBuilder{};
    builder.pushOp<SDFOp::RotateActive>(Vec3::Forward, 0.171264)
        .pushOp<SDFOp::Add, SDF::Sphere>(Vec3::Zero, 0.5, Color::Red)();

    // electrons
    
    for (size_t i{}; i < 4; ++i) {
        double offset{ i * 0.25 };
        builder
            .pushOp<SDFOp::Rotate>(Vec3::One, offset)
            .pushOp<SDFOp::TranslateSine>(Vec3::Left, 1.5, 2.0)
            .pushOp<SDFOp::TranslateSine>(Vec3::Up, 1.5, 2.0)
            .pushOp<SDFOp::TranslateSine>(Vec3::Forward, 1.5, 2.0, 0.25)
            .pushOp<SDFOp::Add, SDF::Sphere>(Vec3::Zero, 0.1)().back(3);
    }

    add(std::move(builder));
    m_lights.push_back(std::make_unique<LightSource>(Vec3{ 1.0, 4.0, -1.0 }, 1.0));
}

void GameEngine::buildMotionDemo()
{
    add(SOBuilder()
        .pushOp<SDFOp::TranslateLinear>(Vec3::Up * 2.0, 1.0, 1.0)
        .pushOp<SDFOp::TranslateLinear>(Vec3::Right * 3.0, 1.0, 1.0, 1.0)
        .pushOp<SDFOp::Add, SDF::Sphere>()()
    );
    m_lights.push_back(std::make_unique<LightSource>(Vec3{ 1.0, 4.0, -1.0 }, 1.0));
}
