#include "BartScene.h"
#include "SDFBuilder.h"

BartScene::BartScene()
{
    Vec3 skinCol = Vec3{ 234.0, 191.0, 4.0 } / 255.0;
    Vec3 shirtCol = Vec3{ 241.0, 78.0, 40.0 } / 255.0;
    Vec3 shortsCol = Vec3{ 0.0, 157.0, 220.0 } / 255.0;

    add(SDFBuilder{}
        .operation<OPTranslate>(Vec3::Up * 3.0)
        .combination<COMBAdd, SDFPlane>(Vec3::Up, -3.8, Color::LightGreen)() // L1
        .stepBack() // L0
        // Head
        .combination<COMBAdd, SDFObject>()() // L1

        // Skull
        .combination<COMBAdd, SDFCylinder>(AXIS_Y, 0.46, 1.2, skinCol)() // L2
        .operation<OPRoundOff>(0.05)

        // Hair
        //.operation<OPMirrorXZ>()
        //.operation<OPRotate>(Vec3::Up, -8.0 / 64.0)
        //.operation<OPMirrorX>()
        //.operation<OPRotate>(Vec3::Up, -4.0 / 64.0)
        //.operation<OPMirrorX>()
        //.operation<OPRotate>(Vec3::Up, -2.0 / 64.0)
        //.operation<OPMirrorX>()
        //.operation<OPRotate>(Vec3::Up, -1.0 / 64.0)
        //.operation<OPTranslate>(Vec3::Forward * 0.51)
        .operation<OPTranslate>(Vec3::Up * 0.7)
        //.operation<OPRotate>(Vec3::Right, -0.06)
        .operation<OPRepeatCircle>(AXIS_Y, 20)
        .operation<OPTranslate>(Vec3::Right * 0.45)
        .combination<COMBAddSmooth, SDFCone>(0.155, 0.2, skinCol)(0.01) // L3
        .operation<OPRoundOff>(0.01)
        .stepBack(2) // L1

        // Upper lip contour
        .operation<OPTranslate>(Vec3{ 0.0, -0.6, -0.12 })
        .combination<COMBAddSmooth, SDFCylinder>(AXIS_Y, 0.35, 0.0, skinCol)(0.15) // L2
        .operation<OPRoundOff>(0.05)
        .stepBack() // L1

        // Lower lip
        .operation<OPTranslate>(Vec3{ 0.0, -0.075, 0.17 })
        .combination<COMBAdd, SDFCylinder>(AXIS_Y, 0.25, 0.05, skinCol)() // L2
        .operation<OPRoundOff>(0.1)
        .stepBack() // L1

        // Neck
        .combination<COMBAddSmooth, SDFObject>()(0.15) // L2
        .operation<OPTranslate>(Vec3{ 0.0, -0.15, 0.2 })
        .operation<OPRotate>(Vec3::Right, 0.04)
        .combination<COMBAdd, SDFCylinder>(AXIS_Y, 0.25, 0.5, skinCol)() // L3
        .stepBack(2) // L1

        // Nose
        .operation<OPTranslate>(Vec3{ 0.0, 0.4, -0.6 })
        .combination<COMBAddSmooth, SDFCapsule>(AXIS_Z, 0.1, 0.2, skinCol)(0.05) // L2
        .stepBack() // L1

        // Eye ridges
        .operation<OPMirrorX>()
        .operation<OPTranslate>(Vec3{ 0.18, 0.35, 0.2 })
        .operation<OPRotate>(Vec3{ 1.0, 0.0, 0.5 }.getNormal(), 0.35)
        .combination<COMBAddSmooth, SDFTorus>(AXIS_Y, Vec2{ 0.20, 0.015 }, skinCol)(0.1) // L2
        .stepBack(0) // L0

        // Eyes
        .combination<COMBAdd, SDFObject>()() //L1
        .operation<OPMirrorX>()
        .operation<OPTranslate>(Vec3{ 0.21, -0.05, -0.4 })
        .combination<COMBAdd, SDFSphere>(0.23)() // L2

        // Pupils
        .operation<OPTranslate>(Vec3{ 0.0, -0.05, -0.13 })
        .combination<COMBAdd, SDFSphere>(0.1, Color::Black)() // L3
        .stepBack(0) // L0

        //// Ears
        .combination<COMBAddSmooth, SDFObject>()(0.05) //L1
        .operation<OPMirrorX>()
        .operation<OPTranslate>(Vec3{ 0.55, -0.35, 0.0 })
        .operation<OPRotate>(Vec3::Down, 0.12)
        .operation<OPRotate>(Vec3::Forward, 0.25)
        .combination<COMBAdd, SDFTorus>(AXIS_Y, Vec2{ .075, 0.02 }, skinCol)() // L2
        .operation<OPTranslate>(Vec3::Down * 0.02)
        .combination<COMBAddSmooth, SDFCylinder>(AXIS_Y, 0.06, 0.01, skinCol)(0.02)
        .stepBack(0) // L0

        // Shirt
        .combination<COMBAdd, SDFObject>()() //L1
        .operation<OPTranslate>(Vec3{ 0.0, -0.2, 0.33 })
        .combination<COMBAdd, SDFCone>(1.4, 0.2, shirtCol)() // L2
        .operation<OPTranslate>(Vec3{ 0.0, -0.4, -0.1 })
        .combination<COMBSubtract, SDFSphere>()() // L2
        .stepBack() // L2
        .operation<OPTranslate>(Vec3{ 0.0, -1.45, 0.1 })
        .combination<COMBAddSmooth, SDFSphere>(0.7, shirtCol)(0.02) //L3
        .operation<OPTranslate>(Vec3::Down * 0.5)
        .combination<COMBSubtract, SDFBox>(Vec3{ 2.0, 1.0, 2.0 })() // L4
        .stepBack() // L3

        // Left sleeve
        .combination<COMBAddSmooth, SDFObject>()(0.01) // L4
        .operation<OPTranslate>(Vec3{ 0.45, 1.3, 0.0 })
        .operation<OPRotate>(Vec3{ -0.5, 0.0, -0.75 }.getNormal(), 0.25)
        .animation<ANSmoothCycle1D>("rotations", 1.0, 0.035)
        .operation<OPTranslate>(Vec3::Down * 0.2)
        .combination<COMBAdd, SDFEllipsoid>(Vec3{ 0.25, 0.4, 0.25 }, shirtCol)() // L5
        .operation<OPTranslate>(Vec3::Down * 0.25)
        .operation<OPOnion>()
        .combination<COMBSubtract, SDFBox>(Vec3{ 1.0, 0.425, 1.0 })() // L6
        .stepBack(2) // L4

        // Left Arm
        .operation<OPRotate>(Vec3::Down, 0.125)
        .operation<OPTranslate>(Vec3::Down * 0.25)
        .combination<COMBAdd, SDFCapsule>(AXIS_Y, 0.17, 0.5, skinCol)() // L5
        .operation<OPTranslate>(Vec3::Down * 0.25)
        .operation<OPRotate>(Vec3::Forward, 0.375)
        .animation<ANSmoothCycle1D>("rotations", 1.0, 0.1, 0.3)
        .operation<OPTranslate>(Vec3::Up * 0.1)
        .combination<COMBAddSmooth, SDFTaperedCapsule>(AXIS_Y, Vec2{ 0.15, 0.1 }, 0.5, skinCol)(0.05) // L6
        // Left hand
        .operation<OPTranslate>(Vec3{ 0.0, 0.6, 0.0 })
        .combination<COMBAddSmooth, SDFEllipsoid>(Vec3{ 0.13, 0.13, 0.05 }, skinCol)(0.05) // L7
        // Left fingers
        .operation<OPTranslate>(Vec3{ 0.0, 0.12, 0.0 })
        .combination<COMBAddSmooth, SDFTaperedCapsule>(AXIS_Y, Vec2{ 0.035, 0.03 }, 0.14, skinCol)(0.05) // L8
        .operation<OPMirrorX>()
        .operation<OPTranslate>(Vec3{ 0.1, -0.02, 0.0 })
        .combination<COMBAddSmooth, SDFTaperedCapsule>(AXIS_Y, Vec2{ 0.035, 0.03 }, 0.125, skinCol)(0.05) // L9
        .stepBack(3) // L6
        // Left thumb
        .operation<OPTranslate>(Vec3{ -0.2, 0.03, 0.0 })
        .operation<OPRotate>(Vec3::Forward, 0.3)
        .combination<COMBAddSmooth, SDFTaperedCapsule>(AXIS_Y, Vec2{ 0.035, 0.03 }, 0.15, skinCol)(0.05) // L7
        .stepBack(4) // L3

        // Right sleeve
        .combination<COMBAddSmooth, SDFObject>()(0.01) // L4
        .operation<OPTranslate>(Vec3{ -0.45, 1.3, 0.0 })
        .operation<OPRotate>(Vec3{ 0.25, 0.0, 1.0 }.getNormal(), 0.06)
        .operation<OPTranslate>(Vec3::Down * 0.2)
        .combination<COMBAdd, SDFEllipsoid>(Vec3{ 0.25, 0.4, 0.25 }, shirtCol)() // L5
        .operation<OPTranslate>(Vec3::Down * 0.25)
        .operation<OPOnion>()
        .combination<COMBSubtract, SDFBox>(Vec3{ 1.0, 0.425, 1.0 })() // L6
        .stepBack(2) // L4

        // Right Arm
        .operation<OPRotate>(Vec3::Down, 0.125)
        .operation<OPTranslate>(Vec3::Down * 0.25)
        .combination<COMBAdd, SDFCapsule>(AXIS_Y, 0.17, 0.5, skinCol)()
        .operation<OPTranslate>(Vec3::Down * 0.25)
        .operation<OPRotate>(Vec3::Forward, 0.45)
        .operation<OPTranslate>(Vec3::Up * 0.1)
        .combination<COMBAddSmooth, SDFTaperedCapsule>(AXIS_Y, Vec2{ 0.15, 0.1 }, 0.5, skinCol)(0.05)
        // Right hand
        .operation<OPTranslate>(Vec3{ 0.0, 0.6, 0.0 })
        .combination<COMBAddSmooth, SDFEllipsoid>(Vec3{ 0.13, 0.13, 0.05 }, skinCol)(0.05)
        // Right fingers
        .operation<OPTranslate>(Vec3{ 0.0, 0.12, 0.0 })
        .combination<COMBAddSmooth, SDFTaperedCapsule>(AXIS_Y, Vec2{ 0.035, 0.03 }, 0.14, skinCol)(0.05)
        .operation<OPMirrorX>()
        .operation<OPTranslate>(Vec3{ 0.1, -0.02, 0.0 })
        .combination<COMBAddSmooth, SDFTaperedCapsule>(AXIS_Y, Vec2{ 0.035, 0.03 }, 0.125, skinCol)(0.05)
        .stepBack(3)
        // Right thumb
        .operation<OPTranslate>(Vec3{ -0.2, 0.03, 0.0 })
        .operation<OPRotate>(Vec3::Forward, 0.3)
        .combination<COMBAddSmooth, SDFTaperedCapsule>(AXIS_Y, Vec2{ 0.035, 0.03 }, 0.15, skinCol)(0.05)
        .stepBack(0)

        // My Shorts (Eat Them)
        .combination<COMBAdd, SDFObject>()() // L1

        // Body
        .operation<OPTranslate>(Vec3{ 0.0, -1.95, 0.35 })
        .combination<COMBAdd, SDFSphere>(0.68, shortsCol)() // L2
        .operation<OPTranslate>(Vec3::Up * 0.9)
        .combination<COMBSubtract, SDFBox>(Vec3{ 2.0 })() // L3
        .stepBack() // L2

        // Pants
        .operation<OPMirrorX>()
        .operation<OPTranslate>(Vec3{ 0.28, -1.4, 0.0 })
        .combination<COMBAddSmooth, SDFCylinder>(AXIS_Y, 0.3, 0.8, shortsCol)(0.1) // L3
        .operation<OPTranslate>(Vec3::Down * 0.55)
        .operation<OPOnion>()
        .combination<COMBSubtract, SDFBox>(Vec3{ 2.0, 0.5, 2.0 })() // L4
        .stepBack(0) // L0

        // Legs
        .combination<COMBAdd, SDFObject>()() // L1
        .operation<OPMirrorX>()
        .operation<OPTranslate>(Vec3{ 0.28, -3.0, 0.3 })
        .combination<COMBAdd, SDFCylinder>(AXIS_Y, 0.15, 1.0, skinCol)() // L2
        .stepBack(0) // L0

        // Shoes
        .combination<COMBAdd, SDFObject>()() // L1
        .operation<OPMirrorX>()
        .operation<OPTranslate>(Vec3{ .28, -3.75, 0.35 })
        .combination<COMBAdd, SDFEllipsoid>(Vec3{ 0.275, 0.42, 0.7 }, shortsCol)() // L2
        .operation<OPTranslate>(Vec3::Backward * 0.2)
        .combination<COMBIntersect, SDFSphere>()() // L3
        .operation<OPTranslate>(Vec3::Down * 0.25)
        .combination<COMBSubtract, SDFBox>(Vec3{ 2.0, 0.5, 2.0 })() // L4
        .stepBack(2) // L2
        .operation<OPTranslate>(Vec3::Forward * 0.2)
        .combination<COMBAdd, SDFEllipsoid>(Vec3{ 0.323, 0.423, 0.73 })() // L2
        .combination<COMBIntersectSmooth, SDFBox>(Vec3{ 2.0, 0.1, 2.0 })(0.0) // L3
        .stepBack() // L2
        .operation<OPTranslate>(Vec3::Backward * 0.25)
        .combination<COMBIntersect, SDFSphere>(0.6)() // L3
        .stepBack() // L2
        .operation<OPTranslate>(Vec3{ 0.0, 0.425, 0.2 })
        .combination<COMBAdd, SDFTorus>(AXIS_Y, Vec2{ 0.17, .055 })() //L3
        .operation<OPTranslate>(Vec3{ -0.22, -0.2, -0.1 })
        .operation<OPRotate>(Vec3::Forward, 0.06)
        .combination<COMBAdd, SDFEllipsoid>(Vec3{ 0.02, 0.1, 0.1 })() // L4
        .stepBack(0)

        .addLightSource(Vec3{ 500.0, 500.0, -1000.0 }, 1500000.0, Color{ 1.0, 1.0, 0.9 }, 0.8)
    );
}
