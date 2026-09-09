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
    buildCubesDemo();
    //buildAtomDemo();
    //buildMotionDemo();
    

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

void GameEngine::buildAtomDemo()
{
    auto builder = SOBuilder{};
    builder
        .pushOp<Add, SceneObject>()();

    // electrons
    
    size_t numAtoms = 1;

    for (size_t i{}; i < numAtoms; ++i) {
        double phase{ i / static_cast<double>(numAtoms) };
        builder
            .pushOp<Translate>(Vec3::Right)
            .pushOp<RotateContinuous>(Vec3::Right, 0.25, phase)
            .pushOp<Add, SceneObject>()()
                .pushOp<Add, Box>(Vec3::Zero, 0.1)()
                    .back(2);
    }
    //builder.pushOp<RotateContinuous>(Vec3::Forward, 0.171264);

    add(std::move(builder));
    add(SOBuilder{}.pushOp<Add, Sphere>(Vec3::Zero, 0.5, Color::Red)());
    m_lights.push_back(std::make_unique<LightSource>(Vec3{ 1.0, 4.0, -1.0 }, 1.0));
}

void GameEngine::buildMotionDemo()
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
