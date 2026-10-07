#include "ExclusionScene.h"
#include "SDFBuilder.h"

ExclusionScene::ExclusionScene()
{
    Vec2 dim{ 0.5, 0.2 };
    double dist{ dim.x * 0.25 };
    double oscillation = 8.0;
    double period = 32.0;
    COMBExclude::ColorMode cMode = COMBExclude::InternalDouble;
    add(SDFBuilder{}
        .comb<COMBAdd, SDFObject>()() // L1
        .op<OPTranslate>(Vec3{ -dist, -dist, 0.0 })
        .modOp<MODOscillateSineV3>("translation", Vec3{ period }, Vec3::Left * dist * oscillation)
        .comb<COMBAdd, SDFTorusXY>(dim, Color::Blue)() // L2
        .op<OPTranslate>(Vec3{ dist * 2.0, 0.0, 0.0 })
        .modOp<MODOscillateSineV3>("translation", Vec3{ period }, Vec3{ dist, dist, 0.0 } * oscillation)
        .comb<COMBExclude, SDFTorusXY>(dim, Color::Green)(cMode) // L3
        .stepBack(0) // L0
        .comb<COMBExclude, SDFObject>()(cMode) // L1
        .op<OPTranslate>(Vec3::Down * dist * 2.0)
        .op<OPTranslate>(Vec3{ -dist, -dist, 0.0 })
        .modOp<MODOscillateSineV3>("translation", Vec3{ period }, Vec3::Down * dist * oscillation)
        .comb<COMBAdd, SDFTorusXY>(dim, Color::Red)() // L2
        .op<OPTranslate>(Vec3{ dist * 2.0, 0.0, 0.0 })
        .modOp<MODOscillateSineV3>("translation", Vec3{ period }, Vec3{ dist, dist, 0.0 } * oscillation)
        .comb<COMBExclude, SDFTorusXY>(dim, Color::Yellow)(cMode) // L4
        .stepBack(0) // L0
        .op<OPTranslate>(Vec3::Backward * 0.5)
        .comb<COMBSubtract, SDFBox>(Vec3{ 100.0, 100.0, 1.0 })() // L1

        .addLightSource(Vec3{ 5.0, 5.0, -10.0 }, 250.0, Color::White, 0.3)
    );
}
