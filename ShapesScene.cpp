#include "ShapesScene.h"

ShapesScene::ShapesScene()
{
    double offset{ 2.0 };

    add(SDFBuilder{}

        .operate<OPTranslate>(Vec3::Left * offset * 2.0 + Vec3::Up)
        .combine<COMBAdd, SDFPlane>(Vec3::Up, -2.0)()
        .combine<COMBAdd, SDFSphere>()().color(Color::Red)
        .operate<OPTranslate>(Vec3::Right * offset)
        .combine<COMBAdd, SDFBox>()().color(Color::Yellow)
        .operate<OPTranslate>(Vec3::Right * offset)
        .combine<COMBAdd, SDFBoxFrame>()().color(Color::Green)
        .operate<OPTranslate>(Vec3::Right * offset)
        .combine<COMBAdd, SDFSphere>()().color(Color::Blue)
        .operate<OPTranslate>(Vec3::Right * offset)
        .combine<COMBAdd, SDFCrystal>()().color(Color::Red)
        .stepBack(0)

        .operate<OPTranslate>(Vec3::Forward * offset)
        .combine<COMBAdd, SDFObject>()()
        .combine<COMBAdd, SDFCone>()().color(Color::Yellow)
        .operate<OPTranslate>(Vec3::Right * offset)
        .combine<COMBAdd, SDFSolidAngle>()().color(Color::Green)
        .operate<OPTranslate>(Vec3::Right * offset)
        .combine<COMBAdd, SDFEllipsoid>()().color(Color::Blue)
        .operate<OPTranslate>(Vec3::Right * offset)
        .combine<COMBAdd, SDFTorus>(AXIS_Y, Vec2{ 0.5, 0.125 })().color(Color::Red)
        .operate<OPTranslate>(Vec3::Right * offset)
        .combine<COMBAdd, SDFTorusSegment>(AXIS_Y, Vec2{ 0.5, 0.125 }, 0.333)().color(Color::Yellow)
        .stepBack(0)

        .operate<OPTranslate>(Vec3::Forward * offset)
        .combine<COMBAdd, SDFObject>()()
        .combine<COMBAdd, SDFCapsule>()().color(Color::Green)
        .operate<OPTranslate>(Vec3::Right * offset)
        .combine<COMBAdd, SDFTaperedCapsule>()().color(Color::Blue)
        .operate<OPTranslate>(Vec3::Right * offset)
        .combine<COMBAdd, SDFCylinder>()().color(Color::Red)
        .operate<OPTranslate>(Vec3::Right * offset)
        .combine<COMBAdd, SDFTaperedCylinder>()().color(Color::Yellow)
        .operate<OPTranslate>(Vec3::Right * offset)
        .combine<COMBAdd, SDFRhombus>()().color(Color::Green)
        .stepBack(0)

        .operate<OPTranslate>(Vec3::Forward * offset)
        .combine<COMBAdd, SDFObject>()()
        .combine<COMBAdd, SDFEquilateralTriangle>()().color(Color::Blue)
        .operate<OPTranslate>(Vec3::Right * offset)
        .combine<COMBAdd, SDFIsoscelesTriangle>()().color(Color::Red)
        .operate<OPTranslate>(Vec3::Right * offset)
        .combine<COMBAdd, SDFTriangle>()().color(Color::Yellow)
        .operate<OPTranslate>(Vec3::Right * offset)
        .combine<COMBAdd, SDFPieSlice>()().color(Color::Green)
        .operate<OPTranslate>(Vec3::Right * offset)
        .combine<COMBAdd, SDFPentagon>()().color(Color::Blue)
        .stepBack(0)

        .operate<OPTranslate>(Vec3::Forward * offset)
        .combine<COMBAdd, SDFObject>()()
        .combine<COMBAdd, SDFHexagon>()().color(Color::Red)
        .operate<OPTranslate>(Vec3::Right * offset * 2)
        .combine<COMBAdd, SDFOctagon>()().color(Color::Green)
        .operate<OPTranslate>(Vec3::Right * offset * 2)
        .combine<COMBAdd, SDFStar>()().color(Color::Red)
        .stepBack(0)

        .addLightSource(Vec3{ 8.0, 10.0, -10.0 }, 200.0, Color::White, 0.25)
    );
}
