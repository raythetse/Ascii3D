#include "WalkingCubesScene.h"
#include "SDFBuilder.h"

WalkingCubesScene::WalkingCubesScene()
{
    double smoothing{ 0.15 };
    double colorBlending{ 0.95 };

    add(SDFBuilder()
        .pushOp<COMBAdd, SDFObject>()()
        .pushOp<OPCubeRollHeight>(1.0, 1.0, 0.5)
        .pushOp<OPTranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
        .pushOp<OPTranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
        .pushOp<OPCubeRoller>(std::vector{ Vec3::Forward, Vec3::Right, Vec3::Backward, Vec3::Left }, 1.0, 0.5)
        .pushOp<COMBAdd, SDFBox>()().color(Color::Red)
        .stepBack(2)
        .pushOp<COMBAddSmooth, SDFObject>()(smoothing, colorBlending)
        .pushOp<OPTranslate>()
        .pushOp<OPCubeRollHeight>(3.0, 1.0, 0.5)
        .pushOp<OPTranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
        .pushOp<OPTranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
        .pushOp<OPCubeRoller>(std::vector{ Vec3::Backward, Vec3::Left, Vec3::Forward, Vec3::Right }, 1.0, 0.5)
        .pushOp<COMBAdd, SDFBox>()().color(Color::Yellow)
        .stepBack(2)
        .pushOp<COMBAddSmooth, SDFObject>()(smoothing, colorBlending)
        .pushOp<OPRotate>()
        .pushOp<OPCubeRollHeight>(1.0, 1.0, 0.5)
        .pushOp<OPTranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
        .pushOp<OPTranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
        .pushOp<OPCubeRoller>(std::vector{ Vec3::Forward, Vec3::Right, Vec3::Backward, Vec3::Left }, 1.0, 0.5)
        .pushOp<COMBAdd, SDFBox>()().color(Color::Blue)
        .stepBack(2)
        .pushOp<COMBAddSmooth, SDFObject>()(smoothing, colorBlending)
        .pushOp<OPTranslate>()
        .pushOp<OPRotate>()
        .pushOp<OPCubeRollHeight>(3.0, 1.0, 0.5)
        .pushOp<OPTranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
        .pushOp<OPTranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
        .pushOp<OPCubeRoller>(std::vector{ Vec3::Backward, Vec3::Left, Vec3::Forward, Vec3::Right }, 1.0, 0.5)
        .pushOp<COMBAdd, SDFBox>()().color(Color::Green)
        .stepBack(2)
        .addLightSource(Vec3{ 2.0, 6.0, -2.0 }, 100.0, Color::White)
    );
}
