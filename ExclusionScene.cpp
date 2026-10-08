#include "ExclusionScene.h"
#include "SDFBuilder.h"

ExclusionScene::ExclusionScene()
{
    Vec2 dim{ 0.5, 0.2 };
    double dist{ dim.x * 0.25 };
    double oscillation = 8.0;
    double period = 32.0;
    ColorMode cMode = COLORMODE_INTERNALDOUBLE;
    add(SDFBuilder{}
        .comb<COMBAdd, SDFObject>()() // L1
        .op<OPTranslate>(Vec3{ -dist, -dist, 0.0 })
        .modOp<MODWaveSmoothV3>("translation", Vec3{ period }, Vec3::Left * dist * oscillation)
        .comb<COMBAdd, SDFTorus>(AXIS_Z, dim, Color::Blue)() // L2
        .op<OPTranslate>(Vec3{ dist * 2.0, 0.0, 0.0 })
        .modOp<MODWaveSmoothV3>("translation", Vec3{ period }, Vec3{ dist, dist, 0.0 } * oscillation)
        .comb<COMBExclude, SDFTorus>(AXIS_Z, dim, Color::Green)(cMode) // L3
        .stepBack(0) // L0
        .comb<COMBExclude, SDFObject>()(cMode) // L1
        .op<OPTranslate>(Vec3::Down * dist * 2.0)
        .op<OPTranslate>(Vec3{ -dist, -dist, 0.0 })
        .modOp<MODWaveSmoothV3>("translation", Vec3{ period }, Vec3::Down * dist * oscillation)
        .comb<COMBAdd, SDFTorus>(AXIS_Z, dim, Color::Red)() // L2
        .op<OPTranslate>(Vec3{ dist * 2.0, 0.0, 0.0 })
        .modOp<MODWaveSmoothV3>("translation", Vec3{ period }, Vec3{ dist, dist, 0.0 } * oscillation)
        .comb<COMBExclude, SDFTorus>(AXIS_Z, dim, Color::Yellow)(cMode) // L4
        .stepBack(0) // L0
        .op<OPTranslate>(Vec3::Backward * 0.5)
        .comb<COMBSubtract, SDFBox>(Vec3{ 100.0, 100.0, 1.0 })() // L1

        .addLightSource(Vec3{ 5.0, 5.0, -10.0 }, 250.0, Color::White, 0.3)
    );
}
