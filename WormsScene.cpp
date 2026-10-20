#include "WormsScene.h"
#include "SDFBuilder.h"

WormsScene::WormsScene()
{
    double rps = 0.25;
    double offset = 1.0 + std::sqrt(2.0);
    Vec2 radii = SDFTorusSegment::DFLT_RADII;

    add(SDFBuilder{}
        .operate<OPTranslate>(Vec3{ 0.0, 2.0, 2.0 })
        .combine<COMBAdd, SDFPlane>(Vec3::Up, -2.0, Color::LightBlue)() //L1
        .combine<COMBAdd, SDFObject>()() //L2
        .operate<OPTranslate>(Vec3::Backward * offset)
        .operate<OPRotate>(Vec3::Forward)
        .animate<ANContinuous1D>("rotations", rps)
        .combine<COMBAdd, SDFTorusSegment>(AXIS_Z, radii, 0.5)() //L3
        .stepBack(2) //L1

        .combine<COMBAdd, SDFObject>()() //L2
        .operate<OPTranslate>(Vec3::Right * offset)
        .operate<OPRotate>(Vec3::Left)
        .animate<ANContinuous1D>("rotations", rps, 0.75 / rps)
        .combine<COMBAdd, SDFTorusSegment>(AXIS_X, radii, 0.5)() //L3
        .stepBack(2) //L1

        .combine<COMBAdd, SDFObject>()() //L2
        .operate<OPTranslate>(Vec3::Forward * offset)
        .operate<OPRotate>(Vec3::Backward)
        .animate<ANContinuous1D>("rotations", rps)
        .combine<COMBAdd, SDFTorusSegment>(AXIS_Z, radii, 0.5)() //L3
        .stepBack(2) //L1

        .combine<COMBAdd, SDFObject>()() //L2
        .operate<OPTranslate>(Vec3::Left * offset)
        .operate<OPRotate>(Vec3::Right)
        .animate<ANContinuous1D>("rotations", rps, 0.25 / rps)
        .combine<COMBAdd, SDFTorusSegment>(AXIS_X, radii, 0.5)() //L3
        .stepBack(2) //L1

        .operate<OPTranslate>(Vec3::Down)
        .combine<COMBSubtract, SDFBox>(Vec3{ 8.0, 2.0, 8.0 })() //L2
        .stepBack(0) //L0

        .combine<COMBAdd, SDFObject>()() //L1
        .operate<OPRotate>(Vec3::Up, 0.125)
        .combine<COMBAdd, SDFObject>()() //L2
        .operate<OPTranslate>(Vec3::Backward * offset)
        .operate<OPRotate>(Vec3::Backward)
        .animate<ANContinuous1D>("rotations", rps)
        .combine<COMBAdd, SDFTorusSegment>(AXIS_Z, radii, 0.5)() //L3
        .stepBack(2) //L1

        .combine<COMBAdd, SDFObject>()() //L2
        .operate<OPTranslate>(Vec3::Right * offset)
        .operate<OPRotate>(Vec3::Right)
        .animate<ANContinuous1D>("rotations", rps, 0.25 / rps)
        .combine<COMBAdd, SDFTorusSegment>(AXIS_X, radii, 0.5)() //L3
        .stepBack(2) //L1

        .combine<COMBAdd, SDFObject>()() //L2
        .operate<OPTranslate>(Vec3::Forward * offset)
        .operate<OPRotate>(Vec3::Forward)
        .animate<ANContinuous1D>("rotations", rps)
        .combine<COMBAdd, SDFTorusSegment>(AXIS_Z, radii, 0.5)() //L3
        .stepBack(2) //L1

        .combine<COMBAdd, SDFObject>()() //L2
        .operate<OPTranslate>(Vec3::Left * offset)
        .operate<OPRotate>(Vec3::Left)
        .animate<ANContinuous1D>("rotations", rps, 0.75 / rps)
        .combine<COMBAdd, SDFTorusSegment>(AXIS_X, radii, 0.5)() //L3
        .stepBack(2) //L1

        .operate<OPTranslate>(Vec3::Up)
        .combine<COMBSubtract, SDFBox>(Vec3{ 8.0, 2.0, 8.0 })() //L2
        .stepBack(0) //L0

        .addLightSource(Vec3{ 3.0, 5.0, -5.0 }, 50.0, Color::LightBlue, 0.25)
        .addLightSource(Vec3{ -2.0, 4.0, -5.0 }, 50.0, Color::Pink, 0.25)
    );
}
