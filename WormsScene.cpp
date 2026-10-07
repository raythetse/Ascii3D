#include "WormsScene.h"
#include "SDFBuilder.h"

WormsScene::WormsScene()
{
    double rps = 0.25;
    double offset = 1.0 + std::sqrt(2.0);
    Vec2 radii = SDFTorusSegment::DFLT_RADII;

    add(SDFBuilder{}
        .comb<COMBAdd, SDFPlane>(Vec3::Up, -2.0, Color::LightBlue)() //L1
        .comb<COMBAdd, SDFObject>()() //L2
        .op<OPTranslate>(Vec3::Backward * offset)
        .op<OPRotateContinuous>(Vec3::Forward, rps)
        .comb<COMBAdd, SDFTorusSegment>(AXIS_Z, radii, 0.5)() //L3
        .stepBack(2) //L1

        .comb<COMBAdd, SDFObject>()() //L2
        .op<OPTranslate>(Vec3::Right * offset)
        .op<OPRotateContinuous>(Vec3::Left, rps, .75)
        .comb<COMBAdd, SDFTorusSegment>(AXIS_X, radii, 0.5)() //L3
        .stepBack(2) //L1

        .comb<COMBAdd, SDFObject>()() //L2
        .op<OPTranslate>(Vec3::Forward * offset)
        .op<OPRotateContinuous>(Vec3::Backward, rps)
        .comb<COMBAdd, SDFTorusSegment>(AXIS_Z, radii, 0.5)() //L3
        .stepBack(2) //L1

        .comb<COMBAdd, SDFObject>()() //L2
        .op<OPTranslate>(Vec3::Left * offset)
        .op<OPRotateContinuous>(Vec3::Right, rps, 0.25)
        .comb<COMBAdd, SDFTorusSegment>(AXIS_X, radii, 0.5)() //L3
        .stepBack(2) //L1

        .op<OPTranslate>(Vec3::Down)
        .comb<COMBSubtract, SDFBox>(Vec3{ 8.0, 2.0, 8.0 })() //L2
        .stepBack(0) //L0

        .comb<COMBAdd, SDFObject>()() //L1
        .op<OPRotate>(Vec3::Up, 0.125)
        .comb<COMBAdd, SDFObject>()() //L2
        .op<OPTranslate>(Vec3::Backward * offset)
        .op<OPRotateContinuous>(Vec3::Backward, rps)
        .comb<COMBAdd, SDFTorusSegment>(AXIS_Z, radii, 0.5)() //L3
        .stepBack(2) //L1

        .comb<COMBAdd, SDFObject>()() //L2
        .op<OPTranslate>(Vec3::Right * offset)
        .op<OPRotateContinuous>(Vec3::Right, rps, .25)
        .comb<COMBAdd, SDFTorusSegment>(AXIS_X, radii, 0.5)() //L3
        .stepBack(2) //L1

        .comb<COMBAdd, SDFObject>()() //L2
        .op<OPTranslate>(Vec3::Forward * offset)
        .op<OPRotateContinuous>(Vec3::Forward, rps)
        .comb<COMBAdd, SDFTorusSegment>(AXIS_Z, radii, 0.5)() //L3
        .stepBack(2) //L1

        .comb<COMBAdd, SDFObject>()() //L2
        .op<OPTranslate>(Vec3::Left * offset)
        .op<OPRotateContinuous>(Vec3::Left, rps, 0.75)
        .comb<COMBAdd, SDFTorusSegment>(AXIS_X, radii, 0.5)() //L3
        .stepBack(2) //L1

        .op<OPTranslate>(Vec3::Up)
        .comb<COMBSubtract, SDFBox>(Vec3{ 8.0, 2.0, 8.0 })() //L2
        .stepBack(0) //L0

        .addLightSource(Vec3{ 3.0, 5.0, -5.0 }, 50.0, Color::LightBlue, 0.25)
        .addLightSource(Vec3{ -2.0, 4.0, -5.0 }, 50.0, Color::Pink, 0.25)
    );
}
