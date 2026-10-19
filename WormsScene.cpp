#include "WormsScene.h"
#include "SDFBuilder.h"

WormsScene::WormsScene()
{
    double rps = 0.25;
    double offset = 1.0 + std::sqrt(2.0);
    Vec2 radii = SDFTorusSegment::DFLT_RADII;

    add(SDFBuilder{}
        .operation<OPTranslate>(Vec3{ 0.0, 2.0, 2.0 })
        .combination<COMBAdd, SDFPlane>(Vec3::Up, -2.0, Color::LightBlue)() //L1
        .combination<COMBAdd, SDFObject>()() //L2
        .operation<OPTranslate>(Vec3::Backward * offset)
        .operation<OPRotate>(Vec3::Forward)
        .animation<ANContinuous1D>("rotations", rps)
        .combination<COMBAdd, SDFTorusSegment>(AXIS_Z, radii, 0.5)() //L3
        .stepBack(2) //L1

        .combination<COMBAdd, SDFObject>()() //L2
        .operation<OPTranslate>(Vec3::Right * offset)
        .operation<OPRotate>(Vec3::Left)
        .animation<ANContinuous1D>("rotations", rps, 0.75 / rps)
        .combination<COMBAdd, SDFTorusSegment>(AXIS_X, radii, 0.5)() //L3
        .stepBack(2) //L1

        .combination<COMBAdd, SDFObject>()() //L2
        .operation<OPTranslate>(Vec3::Forward * offset)
        .operation<OPRotate>(Vec3::Backward)
        .animation<ANContinuous1D>("rotations", rps)
        .combination<COMBAdd, SDFTorusSegment>(AXIS_Z, radii, 0.5)() //L3
        .stepBack(2) //L1

        .combination<COMBAdd, SDFObject>()() //L2
        .operation<OPTranslate>(Vec3::Left * offset)
        .operation<OPRotate>(Vec3::Right)
        .animation<ANContinuous1D>("rotations", rps, 0.25 / rps)
        .combination<COMBAdd, SDFTorusSegment>(AXIS_X, radii, 0.5)() //L3
        .stepBack(2) //L1

        .operation<OPTranslate>(Vec3::Down)
        .combination<COMBSubtract, SDFBox>(Vec3{ 8.0, 2.0, 8.0 })() //L2
        .stepBack(0) //L0

        .combination<COMBAdd, SDFObject>()() //L1
        .operation<OPRotate>(Vec3::Up, 0.125)
        .combination<COMBAdd, SDFObject>()() //L2
        .operation<OPTranslate>(Vec3::Backward * offset)
        .operation<OPRotate>(Vec3::Backward)
        .animation<ANContinuous1D>("rotations", rps)
        .combination<COMBAdd, SDFTorusSegment>(AXIS_Z, radii, 0.5)() //L3
        .stepBack(2) //L1

        .combination<COMBAdd, SDFObject>()() //L2
        .operation<OPTranslate>(Vec3::Right * offset)
        .operation<OPRotate>(Vec3::Right)
        .animation<ANContinuous1D>("rotations", rps, 0.25 / rps)
        .combination<COMBAdd, SDFTorusSegment>(AXIS_X, radii, 0.5)() //L3
        .stepBack(2) //L1

        .combination<COMBAdd, SDFObject>()() //L2
        .operation<OPTranslate>(Vec3::Forward * offset)
        .operation<OPRotate>(Vec3::Forward)
        .animation<ANContinuous1D>("rotations", rps)
        .combination<COMBAdd, SDFTorusSegment>(AXIS_Z, radii, 0.5)() //L3
        .stepBack(2) //L1

        .combination<COMBAdd, SDFObject>()() //L2
        .operation<OPTranslate>(Vec3::Left * offset)
        .operation<OPRotate>(Vec3::Left)
        .animation<ANContinuous1D>("rotations", rps, 0.75 / rps)
        .combination<COMBAdd, SDFTorusSegment>(AXIS_X, radii, 0.5)() //L3
        .stepBack(2) //L1

        .operation<OPTranslate>(Vec3::Up)
        .combination<COMBSubtract, SDFBox>(Vec3{ 8.0, 2.0, 8.0 })() //L2
        .stepBack(0) //L0

        .addLightSource(Vec3{ 3.0, 5.0, -5.0 }, 50.0, Color::LightBlue, 0.25)
        .addLightSource(Vec3{ -2.0, 4.0, -5.0 }, 50.0, Color::Pink, 0.25)
    );
}
