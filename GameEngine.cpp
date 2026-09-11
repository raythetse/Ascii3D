#include "GameEngine.h"
#include <execution>
#include <ranges>
#include "GameTimer.h"
#include "PropertyModifier.h"
#include "SDFOperationInc.h"
#include "SDFBuilder.h"
#include "MathFunctions.h"

void GameEngine::add(SDFBuilder& obj)
{
    m_objects.push_back(obj.get());
}

void GameEngine::add(SDFBuilder&& obj)
{
    m_objects.push_back(obj.get());
}

static int i{};
GameEngine::GameEngine(size_t width, size_t height, double sceneDepth, double stepFactor, double tolerance)
	: m_dimensions{ width, height } , m_sceneDepth{ sceneDepth }, m_pixelField{ width, height }, m_rayMarcher{ sceneDepth, stepFactor, tolerance }, m_renderer{ width * height }
	, m_camera{ width * .005, height * .01 }, m_pixelRange{ std::views::iota(size_t(0), width * height) }, m_objectRange{ std::views::iota(size_t(0), width * height) }
{
    //gyroDemo();
    //oxygenDemo();
    //cubesDemo();
    //propModDemo();
    //frameDemo();
    elongateDemo();
    
    m_objectRange = std::views::iota(size_t(0), m_objects.size());
    GameTimer::reset();
}

void GameEngine::update()
{
    GameTimer::tick();
    m_camera.update();
    for (auto& obj : m_objects) {
        obj->update();
    }
}

void GameEngine::march()
{
    m_rayMarcher.march(m_pixelField, m_objects, m_lights, m_camera, m_pixelRange);
}

void GameEngine::render()
{
    m_renderer.render(m_pixelField);
    m_pixelField.swap();
}

void GameEngine::gyroDemo()
{
    add(SDFBuilder{}
        .pushOp<Add, Torus>(2.0, 0.25)()
    );

    m_lights.push_back(std::make_unique<LightSource>(Vec3{ 1.0, 4.0, -1.0 }, 0.5));
}

void GameEngine::frameDemo()
{
    add(SDFBuilder{}.pushOp<Translate>(Vec3::Down * 4.0).pushOp<Add, Box>(Vec3{ 25.0, 0.0, 25.0 }, Color::SkyBlue)());

    add(SDFBuilder{}
        .pushOp<RotateContinuous>(Vec3::Up, 0.1)
        .pushOp<RotateContinuous>(Vec3::Right, 0.1)
        .pushOp<RotateContinuous>(Vec3::Forward, 0.1)
        .pushOp<Add, BoxFrame>(Vec3{ 2.0 }, 0.1, Color::Gold)()
            .pushOp<TranslateSine>(Vec3::Up, 2.0)
            .pushOp<RotateContinuous>(Vec3::Down, 0.1)
            .pushOp<RotateContinuous>(Vec3::Left, 0.1)
            .pushOp<RotateContinuous>(Vec3::Backward, 0.1)
                .pushOp<Add, SDFObject>()()
                    .pushOp<Add, Box>(Vec3::One, Color::Red, Vec3{ 0.5, 0.5, 0.5 })().stepBack()
                    .pushOp<Add, Box>(Vec3::One, Color::Orange, Vec3{ 0.5, 0.5, -0.5 })().stepBack()
                    .pushOp<Add, Box>(Vec3::One, Color::Magenta, Vec3{ 0.5, -0.5, 0.5 })().stepBack()
                    .pushOp<Add, Box>(Vec3::One, Color::MediumGray, Vec3{ 0.5, -0.5, -0.5 })().stepBack()
                    .pushOp<Add, Box>(Vec3::One, Color::Green, Vec3{ -0.5, 0.5, 0.5 })().stepBack()
                    .pushOp<Add, Box>(Vec3::One, Color::Yellow, Vec3{ -0.5, 0.5, -0.5 })().stepBack()
                    .pushOp<Add, Box>(Vec3::One, Color::Blue, Vec3{ -0.5, -0.5, 0.5 })().stepBack()
                    .pushOp<Add, Box>(Vec3::One, Color::Violet, Vec3{ -0.5, -0.5, -0.5 })().stepBack()
                    .pushOp<Add, BoxFrame>(Vec3{ 1.25 }, 0.1, Color::White)()
        
    );

    m_lights.push_back(std::make_unique<LightSource>(Vec3{ 1.0, 4.0, -1.0 }, 0.5));
}

void GameEngine::oxygenDemo()
{
    auto builder = SDFBuilder{};
    
    size_t numElectrons = 8;

    for (size_t i{}; i < numElectrons; ++i) {
        double phase{ i * MF::TAU };
        double dist = static_cast<double>(i) / static_cast<double>(numElectrons);
        dist *= dist * dist;
        dist = 1.5 + dist;
        double rotPerSec = i % 2 ? 0.5 : -0.5 ;
        builder
            .pushOp<Add, SDFObject>()()
                .pushOp<RotateContinuous>(Vec3::One, 0.01, phase)
                .pushOp<RotateContinuous>(Vec3::Up, rotPerSec, phase)
                .pushOp<Translate>(Vec3::Right * dist)
                .pushOp<Add, Sphere>(0.1, Color::Yellow)()
                    .stepBack(2);
    }

    add(std::move(builder));
    add(SDFBuilder{}
        .pushOp<RotateContinuous>(Vec3::Up, 0.25)
        .pushOp<Add, Sphere>(0.25, Color::Red)().pushOp<Translate>(Vec3{ 0.224, 0.2468, -0.18 })
            .pushOp<Add, Sphere>(0.25, Color::Blue)().pushOp<Translate>(Vec3{ -0.1287, -0.14864, .167 })
                .pushOp<Add, Sphere>(0.25, Color::Red)().pushOp<Translate>(Vec3{ 0.187, -0.0458, 0.2 })
                    .pushOp<Add, Sphere>(0.25, Color::Blue)().pushOp<Translate>(Vec3{ -0.054, -0.217, -0.17 })
                        .pushOp<Add, Sphere>(0.25, Color::Red)().pushOp<Translate>(Vec3{ -.1, -.2256, .076 })
                            .pushOp<Add, Sphere>(0.25, Color::Blue)().pushOp<Translate>(Vec3{ 0.0, 0.26, 0.0 })
                                .pushOp<Add, Sphere>(0.25, Color::Red)().pushOp<Translate>(Vec3{ -0.2, 0.01, 0.0 })
                                    .pushOp<Add, Sphere>(0.25, Color::Blue)().pushOp<Translate>(Vec3{ 0.19, -0.084, -0.02 })
    );

    m_lights.push_back(std::make_unique<LightSource>(Vec3{ 1.0, 4.0, -1.0 }, 0.5));
}

void GameEngine::cubesDemo()
{
    double smoothing{ 0.1 };
    double colorBlending{ 0.95 };

    add(SDFBuilder()
        .pushOp<Add, SDFObject>()()
            .pushOp<CubeRollHeight>(1.0, 1.0, 0.5)
            .pushOp<TranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
            .pushOp<TranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
            .pushOp<CubeRoller>(std::vector{ Vec3::Forward, Vec3::Right, Vec3::Backward, Vec3::Left }, 1.0, 0.5)
            .pushOp<Add, Box>()().color(Color::Red)
                .stepBack(2)
        .pushOp<AddSmooth, SDFObject>()(smoothing, colorBlending)
            .pushOp<Translate>()
            .pushOp<CubeRollHeight>(3.0, 1.0, 0.5)
            .pushOp<TranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
            .pushOp<TranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
            .pushOp<CubeRoller>(std::vector{ Vec3::Backward, Vec3::Left, Vec3::Forward, Vec3::Right }, 1.0, 0.5)
            .pushOp<Add, Box>()().color(Color::Yellow)
                .stepBack(2)
        .pushOp<AddSmooth, SDFObject>()(smoothing, colorBlending)
            .pushOp<Rotate>()
            .pushOp<CubeRollHeight>(1.0, 1.0, 0.5)
            .pushOp<TranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
            .pushOp<TranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
            .pushOp<CubeRoller>(std::vector{ Vec3::Forward, Vec3::Right, Vec3::Backward, Vec3::Left }, 1.0, 0.5)
            .pushOp<Add, Box>()().color(Color::Blue)
                .stepBack(2)
        .pushOp<AddSmooth, SDFObject>()(smoothing, colorBlending)
            .pushOp<Translate>()
            .pushOp<Rotate>()
            .pushOp<CubeRollHeight>(3.0, 1.0, 0.5)
            .pushOp<TranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
            .pushOp<TranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
            .pushOp<CubeRoller>(std::vector{ Vec3::Backward, Vec3::Left, Vec3::Forward, Vec3::Right }, 1.0, 0.5)
            .pushOp<Add, Box>()().color(Color::Green)
    );

    m_lights.push_back(std::make_unique<LightSource>(Vec3{ 1.0, 4.0, -1.0 }, 0.5));
}

void GameEngine::propModDemo()
{
    add(SDFBuilder()
        .pushOp<Add, Sphere>(1.0, Color::Blue)()
        .pushOp<Translate>(Vec3::Left)
        .pushOp<AddSmooth, Sphere>(1.0, Color::Red)()
        .bindPropMod<PMSineOscillation>("colorBlending")
        .pushOp<Translate>(Vec3::Right)
    );

    m_lights.push_back(std::make_unique<LightSource>(Vec3{ 1.0, 4.0, -1.0 }, 0.5));
}

void GameEngine::elongateDemo()
{
    add(SDFBuilder{}
        .pushOp<ElongateY>(0.5)
        .bindPropMod<PMSineOscillation>("length", 1.0, 0.5)
        .pushOp<ElongateZ>(1.0)
        .bindPropMod<PMSineOscillation>("length", 1.348, 1.0)
        .pushOp<ElongateX>(1.0)
        .bindPropMod<PMSineOscillation>("length", 1.783, 1.0)
        .pushOp<Add, Crystal>()()
    );

    m_lights.push_back(std::make_unique<LightSource>(Vec3{ 3.0, 4.0, -3.0 }, 0.25));
}
