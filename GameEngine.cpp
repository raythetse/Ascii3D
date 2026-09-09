#include "GameEngine.h"
#include <execution>
#include <ranges>
#include "GameTimer.h"
#include "Operation.h"
#include "SceneObjectBuilder.h"
#include "MathFunctions.h"

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
    //gyroDemo();
    oxygenDemo();
    //cubesDemo();
    
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

void GameEngine::frameDemo()
{
    add(SOBuilder{}.pushOp<Add, Box>(Vec3::Down * 3.0, Vec3::Zero, Vec3{ 25.0, 0.0, 25.0 }, Color::SkyBlue)());
    add(SOBuilder{}
        .pushOp<RotateContinuous>(Vec3::Up, 0.1)
        .pushOp<RotateContinuous>(Vec3::Right, 0.1)
        .pushOp<RotateContinuous>(Vec3::Forward, 0.1)
        .pushOp<Add, BoxFrame>(Vec3::Up, Vec3::Zero, Vec3{ 1.75 }, 0.1, Color::White)().back()
         .pushOp<TranslateSine>(Vec3::Up, 2.0)
         .pushOp<RotateContinuous>(Vec3::Down, 0.1)
         .pushOp<RotateContinuous>(Vec3::Left, 0.1)
         .pushOp<RotateContinuous>(Vec3::Backward, 0.1)
         .pushOp<Add, Box>(Vec3{ 0.5, -0.5, -0.5 }, Vec3::Zero, Vec3::One, Color::Red)()
          .pushOp<Add, Box>(Vec3{ -0.5, -0.5, 0.5 }, Vec3::Zero, Vec3::One, Color::Orange)()
           .pushOp<Add, Box>(Vec3{ -0.5, 0.5, -0.5 }, Vec3::Zero, Vec3::One, Color::Magenta)()
            .pushOp<Add, Box>(Vec3{ 0.5, 0.5, 0.5 }, Vec3::Zero, Vec3::One, Color::MediumGray)().back(3).pushOp<RoundOff>(0.1)
             .pushOp<Add, Box>(Vec3{ -0.5, -0.5, -0.5 }, Vec3::Zero, Vec3::One, Color::Green)()
              .pushOp<Add, Box>(Vec3{ 0.5, -0.5, 0.5 }, Vec3::Zero, Vec3::One, Color::Yellow)()
               .pushOp<Add, Box>(Vec3{ -0.5, 0.5, 0.5 }, Vec3::Zero, Vec3::One, Color::Blue)()
                .pushOp<Add, Box>(Vec3{ 0.5, 0.5, -0.5 }, Vec3::Zero, Vec3::One, Color::Violet)()
    );
    m_lights.push_back(std::make_unique<LightSource>(Vec3{ 1.0, 4.0, -1.0 }, 1.5));
}

void GameEngine::gyroDemo() {

    m_lights.push_back(std::make_unique<LightSource>(Vec3{ 1.0, 4.0, -1.0 }, 1.5));
}

void GameEngine::oxygenDemo()
{
    auto builder = SOBuilder{};
    
    size_t numElectrons = 8;

    for (size_t i{}; i < numElectrons; ++i) {
        double phase{ i * MF::TAU };
        double dist = static_cast<double>(i) / static_cast<double>(numElectrons);
        dist *= dist * dist;
        dist = 1.5 + dist;
        double rotPerSec = i % 2 ? 0.5 : -0.5 ;
        builder
            .pushOp<Add, SceneObject>()()
            .pushOp<RotateContinuous>(Vec3::One, 0.01, phase)
            .pushOp<RotateContinuous>(Vec3::Up, rotPerSec, phase)
            .pushOp<Translate>(Vec3::Right * dist)
                .pushOp<Add, Sphere>(Vec3::Zero, 0.1, Color::Yellow)()
                    .back(2);
    }
    builder.back().pushOp<RotateContinuous>(Vec3::Forward, 71264);

    add(std::move(builder));
    add(SOBuilder{}
        .pushOp<RotateContinuous>(Vec3::Up, 0.25)
        .pushOp<Add, Sphere>(Vec3{ 0.224, 0.2468, -0.18 }, 0.25, Color::Red)()
        .pushOp<Add, Sphere>(Vec3{ -0.1287, -0.14864, .167 }, 0.25, Color::Blue)()
        .pushOp<Add, Sphere>(Vec3{ 0.187, -0.0458, 0.2 }, 0.25, Color::Red)()
        .pushOp<Add, Sphere>(Vec3{ -0.054, -0.217, -0.17 }, 0.25, Color::Blue)()
        .pushOp<Add, Sphere>(Vec3{ -.1, -.2256, .076 }, 0.25, Color::Red)()
        .pushOp<Add, Sphere>(Vec3{ 0.0, 0.26, 0.0 }, 0.25, Color::Blue)()
        .pushOp<Add, Sphere>(Vec3{ -0.2, 0.01, 0.0 }, 0.25, Color::Red)()
        .pushOp<Add, Sphere>(Vec3{ 0.19, -0.084, -0.02 }, 0.25, Color::Blue)()
    );
    m_lights.push_back(std::make_unique<LightSource>(Vec3{ 1.0, 4.0, -1.0 }, 0.5));
}

void GameEngine::cubesDemo()
{
    double smoothing{ 0.1 };
    double colorBlending{ 0.95 };
    add(SOBuilder()
        .pushOp<Add, SceneObject>()()
         .pushOp<CubeRollHeight>(1.0, 1.0, 0.5)
         .pushOp<TranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
         .pushOp<TranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
         .pushOp<CubeRoller>(std::vector{ Vec3::Forward, Vec3::Right, Vec3::Backward, Vec3::Left }, 1.0, 0.5)
         .pushOp<Add, Box>()().color(Color::Red)
          .back(2)
        .pushOp<AddSmooth, SceneObject>()(smoothing, colorBlending)
         .pushOp<Translate>()
         .pushOp<CubeRollHeight>(3.0, 1.0, 0.5)
         .pushOp<TranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
         .pushOp<TranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
         .pushOp<CubeRoller>(std::vector{ Vec3::Backward, Vec3::Left, Vec3::Forward, Vec3::Right }, 1.0, 0.5)
         .pushOp<Add, Box>()().color(Color::Yellow)
          .back(2)
        .pushOp<AddSmooth, SceneObject>()(smoothing, colorBlending)
         .pushOp<Rotate>()
         .pushOp<CubeRollHeight>(1.0, 1.0, 0.5)
         .pushOp<TranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
         .pushOp<TranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
         .pushOp<CubeRoller>(std::vector{ Vec3::Forward, Vec3::Right, Vec3::Backward, Vec3::Left }, 1.0, 0.5)
         .pushOp<Add, Box>()().color(Color::Blue)
          .back(2)
        .pushOp<AddSmooth, SceneObject>()(smoothing, colorBlending)
         .pushOp<Translate>()
         .pushOp<Rotate>()
         .pushOp<CubeRollHeight>(3.0, 1.0, 0.5)
         .pushOp<TranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
         .pushOp<TranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
         .pushOp<CubeRoller>(std::vector{ Vec3::Backward, Vec3::Left, Vec3::Forward, Vec3::Right }, 1.0, 0.5)
         .pushOp<Add, Box>()().color(Color::Green)
    );

    m_lights.push_back(std::make_unique<LightSource>(Vec3{ 2.0, 4.0, -5.0 }, 1.0));
}
