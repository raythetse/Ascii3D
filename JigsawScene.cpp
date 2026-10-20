#include "JigsawScene.h"

JigsawScene::JigsawScene()
{
    Vec3 repeatVec{ 2.0, 1.0, 2.0 * MF::ROOT3 };
    double nubSize{ 0.3 };

    add(SDFBuilder{}
        // Hegaxons don't tessellate in a square grid, but we can use two grids with vertically spread rows to overcome this
        .combine<COMBAdd, SDFObject>()()
            .combine<COMBAdd, SDFObject>()()
                .operate<OPRepeatXZ>(repeatVec)
                .operate<OPRotate>(Vec3::Up, 1.0 / 12.0 )
                .combine<COMBAdd, SDFHexagon>(OP2D_EXTRUDE, 0.95, 0.1)()
                    .stepBack(3)

        .combine<COMBSubtractSmooth, SDFObject>()(0.025)
            .operate<OPRepeatXZ>(repeatVec)
            .operate<OPRepeatCircle>(AXIS_Y, 3)
            .operate<OPTranslate>(Vec3{ 1.0, 0.0, 0.0 })
            .combine<COMBAdd, SDFCylinder>(AXIS_Y, nubSize + 0.1, 0.2)()
                .stepBack(0)

        .combine<COMBAdd, SDFObject>()()
            .combine<COMBAdd, SDFObject>()()
                .operate<OPTranslate>(Vec3{ 1.0, 0.0, MF::ROOT3 })
                .combine<COMBAdd, SDFObject>()()
                    .operate<OPRepeatXZ>(repeatVec)
                    .operate<OPRotate>(Vec3::Up, 1.0 / 12.0 )
                    .combine<COMBAdd, SDFHexagon>(OP2D_EXTRUDE, 0.95, 0.1)()
                        .stepBack(3)


            .combine<COMBSubtractSmooth, SDFObject>()(0.025)
                .operate<OPTranslate>(Vec3{ 1.0, 0.0, MF::ROOT3 })
                .operate<OPRepeatXZ>(repeatVec)
                .operate<OPRepeatCircle>(AXIS_Y, 3)
                .operate<OPTranslate>(Vec3{ 1.0, 0.0, 0.0 })
                .combine<COMBAdd, SDFCylinder>(AXIS_Y, nubSize + 0.1, 0.2)()
                    .stepBack(0)

        .combine<COMBAdd, SDFObject>()(0.05)
            .operate<OPTranslate>(Vec3{ 1.0, 0.0, 0.0 })
            .operate<OPRepeatXZ>(repeatVec)
            .combine<COMBAdd, SDFCylinder>(AXIS_Y, nubSize, 0.1)()
            .   stepBack(0)

        .combine<COMBAdd, SDFObject>()(0.05)
            .operate<OPTranslate>(Vec3{ 0.5, 0.0, MF::ROOT3 * 0.5 })
            .operate<OPRepeatXZ>(repeatVec)
            .combine<COMBAdd, SDFCylinder>(AXIS_Y, nubSize, 0.1)()
                .stepBack(0)

        .combine<COMBAdd, SDFObject>()(0.05)
            .operate<OPTranslate>(Vec3{ 0.5, 0.0, -MF::ROOT3 * 0.5 })
            .operate<OPRepeatXZ>(repeatVec)
            .combine<COMBAdd, SDFCylinder>(AXIS_Y, nubSize, 0.1)()
                .stepBack(0)

        .combine<COMBAdd, SDFObject>()(0.05)
            .operate<OPTranslate>(Vec3{ 1.0, 0.0, 0.0 })
            .operate<OPTranslate>(Vec3{ 1.0, 0.0, MF::ROOT3 })
            .operate<OPRepeatXZ>(repeatVec)
            .combine<COMBAdd, SDFCylinder>(AXIS_Y, nubSize, 0.1)()
                .stepBack(0)

        .combine<COMBAdd, SDFObject>()(0.05)
            .operate<OPTranslate>(Vec3{ 0.5, 0.0, MF::ROOT3 * 0.5 })
            .operate<OPTranslate>(Vec3{ 1.0, 0.0, MF::ROOT3 })
            .operate<OPRepeatXZ>(repeatVec)
            .combine<COMBAdd, SDFCylinder>(AXIS_Y, nubSize, 0.1)()
                .stepBack(0)

        .combine<COMBAdd, SDFObject>()(0.05)
            .operate<OPTranslate>(Vec3{ 0.5, 0.0, -MF::ROOT3 * 0.5 })
            .operate<OPTranslate>(Vec3{ 1.0, 0.0, MF::ROOT3 })
            .operate<OPRepeatXZ>(repeatVec)
            .combine<COMBAdd, SDFCylinder>(AXIS_Y, nubSize, 0.1)()
                .stepBack(0)

        .operate<OPRoundOff>(0.03)

        .addLightSource(Vec3{ 50.0, 50.0, -50.0 }, 5000.0, Color::White, 0.2)
    );
}
