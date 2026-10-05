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
        .pushOp<COMBAdd, SDFObject>()() // L1
        .pushOp<OPTranslate>(Vec3{ -dist, -dist, 0.0 })
        .bindPropMod<PMOscillateSineV3>("translation", Vec3{ period }, Vec3::Left * dist * oscillation)
        .pushOp<COMBAdd, SDFTorusXY>(dim, Color::Blue)() // L2
        .pushOp<OPTranslate>(Vec3{ dist * 2.0, 0.0, 0.0 })
        .bindPropMod<PMOscillateSineV3>("translation", Vec3{ period }, Vec3{ dist, dist, 0.0 } * oscillation)
        .pushOp<COMBExclude, SDFTorusXY>(dim, Color::Green)(cMode) // L3
        .stepBack(0) // L0
        .pushOp<COMBExclude, SDFObject>()(cMode) // L1
        .pushOp<OPTranslate>(Vec3::Down * dist * 2.0)
        .pushOp<OPTranslate>(Vec3{ -dist, -dist, 0.0 })
        .bindPropMod<PMOscillateSineV3>("translation", Vec3{ period }, Vec3::Down * dist * oscillation)
        .pushOp<COMBAdd, SDFTorusXY>(dim, Color::Red)() // L2
        .pushOp<OPTranslate>(Vec3{ dist * 2.0, 0.0, 0.0 })
        .bindPropMod<PMOscillateSineV3>("translation", Vec3{ period }, Vec3{ dist, dist, 0.0 } * oscillation)
        .pushOp<COMBExclude, SDFTorusXY>(dim, Color::Yellow)(cMode) // L4
        .stepBack(0) // L0
        .pushOp<OPTranslate>(Vec3::Backward * 0.5)
        .pushOp<COMBSubtract, SDFBox>(Vec3{ 100.0, 100.0, 1.0 })() // L1

        .addLightSource(Vec3{ 5.0, 5.0, -10.0 }, 250.0, Color::White, 0.3)
    );
}
