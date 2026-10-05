#include "WormsScene.h"
#include "SDFBuilder.h"

WormsScene::WormsScene()
{
    double rps = 0.25;
    double offset = 1.0 + std::sqrt(2.0);
    Vec2 radii = SDFTorusSegmentXY::DFLT_RADII;

    add(SDFBuilder{}
        .pushOp<COMBAdd, SDFPlane>(Vec3::Up, -2.0, Color::LightBlue)() //L1
        .pushOp<COMBAdd, SDFObject>()() //L2
        .pushOp<OPTranslate>(Vec3::Backward * offset)
        .pushOp<OPRotateContinuous>(Vec3::Forward, rps)
        .pushOp<COMBAdd, SDFTorusSegmentXY>(radii, 0.5)() //L3
        .stepBack(2) //L1

        .pushOp<COMBAdd, SDFObject>()() //L2
        .pushOp<OPTranslate>(Vec3::Right * offset)
        .pushOp<OPRotateContinuous>(Vec3::Left, rps, .75)
        .pushOp<COMBAdd, SDFTorusSegmentYZ>(radii, 0.5)() //L3
        .stepBack(2) //L1

        .pushOp<COMBAdd, SDFObject>()() //L2
        .pushOp<OPTranslate>(Vec3::Forward * offset)
        .pushOp<OPRotateContinuous>(Vec3::Backward, rps)
        .pushOp<COMBAdd, SDFTorusSegmentXY>(radii, 0.5)() //L3
        .stepBack(2) //L1

        .pushOp<COMBAdd, SDFObject>()() //L2
        .pushOp<OPTranslate>(Vec3::Left * offset)
        .pushOp<OPRotateContinuous>(Vec3::Right, rps, 0.25)
        .pushOp<COMBAdd, SDFTorusSegmentYZ>(radii, 0.5)() //L3
        .stepBack(2) //L1

        .pushOp<OPTranslate>(Vec3::Down)
        .pushOp<COMBSubtract, SDFBox>(Vec3{ 8.0, 2.0, 8.0 })() //L2
        .stepBack(0) //L0

        .pushOp<COMBAdd, SDFObject>()() //L1
        .pushOp<OPRotate>(Vec3::Up, 0.125)
        .pushOp<COMBAdd, SDFObject>()() //L2
        .pushOp<OPTranslate>(Vec3::Backward * offset)
        .pushOp<OPRotateContinuous>(Vec3::Backward, rps)
        .pushOp<COMBAdd, SDFTorusSegmentXY>(radii, 0.5)() //L3
        .stepBack(2) //L1

        .pushOp<COMBAdd, SDFObject>()() //L2
        .pushOp<OPTranslate>(Vec3::Right * offset)
        .pushOp<OPRotateContinuous>(Vec3::Right, rps, .25)
        .pushOp<COMBAdd, SDFTorusSegmentYZ>(radii, 0.5)() //L3
        .stepBack(2) //L1

        .pushOp<COMBAdd, SDFObject>()() //L2
        .pushOp<OPTranslate>(Vec3::Forward * offset)
        .pushOp<OPRotateContinuous>(Vec3::Forward, rps)
        .pushOp<COMBAdd, SDFTorusSegmentXY>(radii, 0.5)() //L3
        .stepBack(2) //L1

        .pushOp<COMBAdd, SDFObject>()() //L2
        .pushOp<OPTranslate>(Vec3::Left * offset)
        .pushOp<OPRotateContinuous>(Vec3::Left, rps, 0.75)
        .pushOp<COMBAdd, SDFTorusSegmentYZ>(radii, 0.5)() //L3
        .stepBack(2) //L1

        .pushOp<OPTranslate>(Vec3::Up)
        .pushOp<COMBSubtract, SDFBox>(Vec3{ 8.0, 2.0, 8.0 })() //L2
        .stepBack(0) //L0

        .addLightSource(Vec3{ 3.0, 5.0, -5.0 }, 50.0, Color::LightBlue, 0.25)
        .addLightSource(Vec3{ -2.0, 4.0, -5.0 }, 50.0, Color::Pink, 0.25)
    );
}
