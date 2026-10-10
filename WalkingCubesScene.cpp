#include "WalkingCubesScene.h"
#include "SDFBuilder.h"

WalkingCubesScene::WalkingCubesScene()
{
    double smoothing{ 0.15 };
    double colorBlending{ 0.95 };

    add(SDFBuilder()
        .combination<COMBAdd, SDFObject>()()
        .operation<OPCubeRollHeight>(1.0, 1.0, 0.5)
        .operation<OPTranslate>()
        .animation<ANLinearCycle3D>("translation", Vec3{ 0.5, 0.0, 0.5 }, Vec3{ 1.0, 1.0, 1.0 }, Vec3{ 1.0, 1.0, 1.0 }, Vec3{ 1.0, 1.0, 1.0 }, Vec3{ 1.0, 1.0, 1.0 }, Vec3{ 0.5, 0.0, 1.5 })
        .operation<OPCubeRoller>(std::vector{ Vec3::Forward, Vec3::Right, Vec3::Backward, Vec3::Left }, 1.0, 0.5)
        .combination<COMBAdd, SDFBox>()().color(Color::Red)
        .stepBack(2)
        .combination<COMBAddSmooth, SDFObject>()(smoothing, colorBlending)
        .operation<OPTranslate>()
        .operation<OPTranslate>()
        .animation<ANLinearCycle3D>("translation", Vec3{ 0.5, 0.0, 0.5 }, Vec3{ 1.0, 1.0, 1.0 }, Vec3{ 1.0, 1.0, 1.0 }, Vec3{ 1.0, 1.0, 1.0 }, Vec3{ 1.0, 1.0, 1.0 }, Vec3{ 0.5, 0.0, 1.5 })
        .operation<OPCubeRollHeight>(3.0, 1.0, 0.5)
        .operation<OPCubeRoller>(std::vector{ Vec3::Backward, Vec3::Left, Vec3::Forward, Vec3::Right }, 1.0, 0.5)
        .combination<COMBAdd, SDFBox>()().color(Color::Yellow)
        .stepBack(2)
        .combination<COMBAddSmooth, SDFObject>()(smoothing, colorBlending)
        .operation<OPRotate>()
        .operation<OPCubeRollHeight>(1.0, 1.0, 0.5)
        .operation<OPTranslate>()
        .animation<ANLinearCycle3D>("translation", Vec3{ 0.5, 0.0, 0.5 }, Vec3{ 1.0, 1.0, 1.0 }, Vec3{ 1.0, 1.0, 1.0 }, Vec3{ 1.0, 1.0, 1.0 }, Vec3{ 1.0, 1.0, 1.0 }, Vec3{ 0.5, 0.0, 1.5 })
        .operation<OPCubeRoller>(std::vector{ Vec3::Forward, Vec3::Right, Vec3::Backward, Vec3::Left }, 1.0, 0.5)
        .combination<COMBAdd, SDFBox>()().color(Color::Blue)
        .stepBack(2)
        .combination<COMBAddSmooth, SDFObject>()(smoothing, colorBlending)
        .operation<OPTranslate>()
        .operation<OPRotate>()
        .operation<OPCubeRollHeight>(3.0, 1.0, 0.5)
        .operation<OPTranslate>()
        .animation<ANLinearCycle3D>("translation", Vec3{ 0.5, 0.0, 0.5 }, Vec3{ 1.0, 1.0, 1.0 }, Vec3{ 1.0, 1.0, 1.0 }, Vec3{ 1.0, 1.0, 1.0 }, Vec3{ 1.0, 1.0, 1.0 }, Vec3{ 0.5, 0.0, 1.5 })
        .operation<OPCubeRoller>(std::vector{ Vec3::Backward, Vec3::Left, Vec3::Forward, Vec3::Right }, 1.0, 0.5)
        .combination<COMBAdd, SDFBox>()().color(Color::Green)
        .stepBack(2)
        .addLightSource(Vec3{ 2.0, 6.0, -2.0 }, 50.0, Color::White)
    );
}
