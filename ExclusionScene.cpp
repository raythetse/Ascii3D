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
        .combine<COMBAdd, SDFObject>()()
        .operate<OPTranslate>(Vec3{ -dist, -dist, 0.0 })
        .animate<ANSmoothCycle3D>("translation", Vec3{ period }, Vec3::Left * dist * oscillation)
        .combine<COMBAdd, SDFTorus>(AXIS_Z, dim, Color::Blue)()
        .operate<OPTranslate>(Vec3{ dist * 2.0, 0.0, 0.0 })
        .animate<ANSmoothCycle3D>("translation", Vec3{ period }, Vec3{ dist, dist, 0.0 } * oscillation)
        .combine<COMBExclude, SDFTorus>(AXIS_Z, dim, Color::Green)(cMode)
        .stepBack(0)
        .combine<COMBExclude, SDFObject>()(cMode)
        .operate<OPTranslate>(Vec3::Down * dist * 2.0)
        .operate<OPTranslate>(Vec3{ -dist, -dist, 0.0 })
        .animate<ANSmoothCycle3D>("translation", Vec3{ period }, Vec3::Down * dist * oscillation)
        .combine<COMBAdd, SDFTorus>(AXIS_Z, dim, Color::Red)()
        .operate<OPTranslate>(Vec3{ dist * 2.0, 0.0, 0.0 })
        .animate<ANSmoothCycle3D>("translation", Vec3{ period }, Vec3{ dist, dist, 0.0 } * oscillation)
        .combine<COMBExclude, SDFTorus>(AXIS_Z, dim, Color::Yellow)(cMode)
        .stepBack(0)
        .operate<OPTranslate>(Vec3::Backward * 0.5)
        .combine<COMBSubtract, SDFBox>(Vec3{ 100.0, 100.0, 1.0 })()

        .addLightSource(Vec3{ 5.0, 5.0, -10.0 }, 250.0, Color::White, 0.3)
    );
}
