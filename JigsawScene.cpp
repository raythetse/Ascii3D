#include "JigsawScene.h"

JigsawScene::JigsawScene()
{
    Vec3 repeatVec{ 2.0, 1.0, 2.0 * MF::ROOT3 };
    double nubSize{ 0.3 };

    add(SDFBuilder{}
        // Hegaxons don't tessellate in a square grid, but we can use two grids with vertically spread rows to overcome this
        .combination<COMBAdd, SDFObject>()()
            .combination<COMBAdd, SDFObject>()()
                .operation<OPRepeatXZ>(repeatVec)
                .operation<OPRotate>(Vec3::Up, 1.0 / 12.0 )
                .combination<COMBAdd, SDFHexagon>(OP2D_EXTRUDE, 0.95, 0.1)()
                    .stepBack(3)

        .combination<COMBSubtractSmooth, SDFObject>()(0.025)
            .operation<OPRepeatXZ>(repeatVec)
            .operation<OPRepeatCircle>(AXIS_Y, 3)
            .operation<OPTranslate>(Vec3{ 1.0, 0.0, 0.0 })
            .combination<COMBAdd, SDFCylinder>(AXIS_Y, nubSize + 0.1, 0.2)()
                .stepBack(0)

        .combination<COMBAdd, SDFObject>()()
            .combination<COMBAdd, SDFObject>()()
                .operation<OPTranslate>(Vec3{ 1.0, 0.0, MF::ROOT3 })
                .combination<COMBAdd, SDFObject>()()
                    .operation<OPRepeatXZ>(repeatVec)
                    .operation<OPRotate>(Vec3::Up, 1.0 / 12.0 )
                    .combination<COMBAdd, SDFHexagon>(OP2D_EXTRUDE, 0.95, 0.1)()
                        .stepBack(3)


            .combination<COMBSubtractSmooth, SDFObject>()(0.025)
                .operation<OPTranslate>(Vec3{ 1.0, 0.0, MF::ROOT3 })
                .operation<OPRepeatXZ>(repeatVec)
                .operation<OPRepeatCircle>(AXIS_Y, 3)
                .operation<OPTranslate>(Vec3{ 1.0, 0.0, 0.0 })
                .combination<COMBAdd, SDFCylinder>(AXIS_Y, nubSize + 0.1, 0.2)()
                    .stepBack(0)

        .combination<COMBAdd, SDFObject>()(0.05)
            .operation<OPTranslate>(Vec3{ 1.0, 0.0, 0.0 })
            .operation<OPRepeatXZ>(repeatVec)
            .combination<COMBAdd, SDFCylinder>(AXIS_Y, nubSize, 0.1)()
            .   stepBack(0)

        .combination<COMBAdd, SDFObject>()(0.05)
            .operation<OPTranslate>(Vec3{ 0.5, 0.0, MF::ROOT3 * 0.5 })
            .operation<OPRepeatXZ>(repeatVec)
            .combination<COMBAdd, SDFCylinder>(AXIS_Y, nubSize, 0.1)()
                .stepBack(0)

        .combination<COMBAdd, SDFObject>()(0.05)
            .operation<OPTranslate>(Vec3{ 0.5, 0.0, -MF::ROOT3 * 0.5 })
            .operation<OPRepeatXZ>(repeatVec)
            .combination<COMBAdd, SDFCylinder>(AXIS_Y, nubSize, 0.1)()
                .stepBack(0)

        .combination<COMBAdd, SDFObject>()(0.05)
            .operation<OPTranslate>(Vec3{ 1.0, 0.0, 0.0 })
            .operation<OPTranslate>(Vec3{ 1.0, 0.0, MF::ROOT3 })
            .operation<OPRepeatXZ>(repeatVec)
            .combination<COMBAdd, SDFCylinder>(AXIS_Y, nubSize, 0.1)()
                .stepBack(0)

        .combination<COMBAdd, SDFObject>()(0.05)
            .operation<OPTranslate>(Vec3{ 0.5, 0.0, MF::ROOT3 * 0.5 })
            .operation<OPTranslate>(Vec3{ 1.0, 0.0, MF::ROOT3 })
            .operation<OPRepeatXZ>(repeatVec)
            .combination<COMBAdd, SDFCylinder>(AXIS_Y, nubSize, 0.1)()
                .stepBack(0)

        .combination<COMBAdd, SDFObject>()(0.05)
            .operation<OPTranslate>(Vec3{ 0.5, 0.0, -MF::ROOT3 * 0.5 })
            .operation<OPTranslate>(Vec3{ 1.0, 0.0, MF::ROOT3 })
            .operation<OPRepeatXZ>(repeatVec)
            .combination<COMBAdd, SDFCylinder>(AXIS_Y, nubSize, 0.1)()
                .stepBack(0)

        .operation<OPRoundOff>(0.03)

        .addLightSource(Vec3{ 50.0, 50.0, -50.0 }, 5000.0, Color::White, 0.2)
    );
}
