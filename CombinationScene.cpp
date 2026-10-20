#include "CombinationScene.h"

CombinationScene::CombinationScene()
{
    add(SDFBuilder{}

        .combine<COMBAdd, SDFObject>()()
        .operate<OPTranslate>(Vec3::Left * 2.0)
        .combine<COMBAdd, SDFBox>(1.5)()
        .operate<OPTranslate>(Vec3::Up * 0.85)
        .combine<COMBAdd, SDFCrystal>(0.5)()
        .stepBack(0)

        .combine<COMBAdd, SDFObject>()()
        .operate<OPTranslate>(Vec3::Left * 2.0)
        .operate<OPTranslate>(Vec3::Forward * 2.0)
        .combine<COMBAdd, SDFBox>(1.5)()
        .operate<OPTranslate>(Vec3::Up * 0.85)
        .combine<COMBAddSmooth, SDFCrystal>(0.5)(0.25)
        .animateComb<ANSmoothCycle1D>("smoothing", 2.0, 0.25)
        .stepBack(0)

        .combine<COMBAdd, SDFObject>()()
        .combine<COMBAdd, SDFBox>(1.5)()
        .operate<OPTranslate>(Vec3::Up * 0.85)
        .combine<COMBSubtract, SDFCrystal>(0.5)()
        .stepBack(0)

        .combine<COMBAdd, SDFObject>()()
        .operate<OPTranslate>(Vec3::Forward * 2.0)
        .combine<COMBAdd, SDFBox>(1.5)()
        .operate<OPTranslate>(Vec3::Up * 0.85)
        .combine<COMBSubtractSmooth, SDFCrystal>(0.5)(0.25)
        .animateComb<ANSmoothCycle1D>("smoothing", 2.0, 0.25)
        .stepBack(0)

        .combine<COMBAdd, SDFObject>()()
        .operate<OPTranslate>(Vec3::Right * 2.0)
        .combine<COMBAdd, SDFBox>(1.5)()
        .operate<OPTranslate>(Vec3::Up * 0.85)
        .combine<COMBIntersect, SDFCrystal>(0.5)()
        .stepBack(0)

        .combine<COMBAdd, SDFObject>()()
        .operate<OPTranslate>(Vec3::Right * 2.0)
        .operate<OPTranslate>(Vec3::Forward * 2.0)
        .combine<COMBAdd, SDFBox>(1.5)()
        .operate<OPTranslate>(Vec3::Up * 0.85)
        .combine<COMBIntersectSmooth, SDFCrystal>(0.5)(0.25)
        .animateComb<ANSmoothCycle1D>("smoothing", 2.0, 0.25)
        .stepBack(0)

        .addLightSource(Vec3{ 8.0, 10.0, -10.0 }, 200.0, Color::White, 0.25)
    );
}
