#include "WalkingCubesScene.h"
#include "SDFBuilder.h"

WalkingCubesScene::WalkingCubesScene()
{
    double smoothing{ 0.15 };
    double colorBlending{ 0.95 };

    add(SDFBuilder()
        .comb<COMBAdd, SDFObject>()()
        .op<OPCubeRollHeight>(1.0, 1.0, 0.5)
        .op<OPTranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
        .op<OPTranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
        .op<OPCubeRoller>(std::vector{ Vec3::Forward, Vec3::Right, Vec3::Backward, Vec3::Left }, 1.0, 0.5)
        .comb<COMBAdd, SDFBox>()().color(Color::Red)
        .stepBack(2)
        .comb<COMBAddSmooth, SDFObject>()(smoothing, colorBlending)
        .op<OPTranslate>()
        .op<OPCubeRollHeight>(3.0, 1.0, 0.5)
        .op<OPTranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
        .op<OPTranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
        .op<OPCubeRoller>(std::vector{ Vec3::Backward, Vec3::Left, Vec3::Forward, Vec3::Right }, 1.0, 0.5)
        .comb<COMBAdd, SDFBox>()().color(Color::Yellow)
        .stepBack(2)
        .comb<COMBAddSmooth, SDFObject>()(smoothing, colorBlending)
        .op<OPRotate>()
        .op<OPCubeRollHeight>(1.0, 1.0, 0.5)
        .op<OPTranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
        .op<OPTranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
        .op<OPCubeRoller>(std::vector{ Vec3::Forward, Vec3::Right, Vec3::Backward, Vec3::Left }, 1.0, 0.5)
        .comb<COMBAdd, SDFBox>()().color(Color::Blue)
        .stepBack(2)
        .comb<COMBAddSmooth, SDFObject>()(smoothing, colorBlending)
        .op<OPTranslate>()
        .op<OPRotate>()
        .op<OPCubeRollHeight>(3.0, 1.0, 0.5)
        .op<OPTranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
        .op<OPTranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
        .op<OPCubeRoller>(std::vector{ Vec3::Backward, Vec3::Left, Vec3::Forward, Vec3::Right }, 1.0, 0.5)
        .comb<COMBAdd, SDFBox>()().color(Color::Green)
        .stepBack(2)
        .addLightSource(Vec3{ 2.0, 6.0, -2.0 }, 100.0, Color::White)
    );
}
