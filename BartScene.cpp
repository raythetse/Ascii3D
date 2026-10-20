#include "BartScene.h"
#include "SDFBuilder.h"

BartScene::BartScene()
{
    Vec3 skinCol = Vec3{ 234.0, 191.0, 4.0 } / 255.0;
    Vec3 shirtCol = Vec3{ 241.0, 78.0, 40.0 } / 255.0;
    Vec3 shortsCol = Vec3{ 0.0, 157.0, 220.0 } / 255.0;

    add(SDFBuilder{}
        .operate<OPTranslate>(Vec3::Up * 3.0)
        .combine<COMBAdd, SDFPlane>(Vec3::Up, -3.8, Color::LightGreen)() // L1
        .stepBack() // L0
        // Head
        .combine<COMBAdd, SDFObject>()() // L1

        // Skull
        .combine<COMBAdd, SDFCylinder>(AXIS_Y, 0.46, 1.2, skinCol)() // L2
        .operate<OPRoundOff>(0.05)

        // Hair
        .operate<OPTranslate>(Vec3::Up * 0.8)
        .operate<OPRepeatCircle>(AXIS_Y, 22)
        .operate<OPTranslate>(Vec3::Right * 0.5)
        .operate<OPRotate>(Vec3::Forward, 0.05)
        .combine<COMBAddSmooth, SDFCone>(0.155, 0.2, skinCol)(0.02) // L3
        .operate<OPRoundOff>(0.01)
        .stepBack(2) // L1

        // Upper lip contour
        .operate<OPTranslate>(Vec3{ 0.0, -0.6, -0.12 })
        .combine<COMBAddSmooth, SDFCylinder>(AXIS_Y, 0.35, 0.0, skinCol)(0.15) // L2
        .operate<OPRoundOff>(0.05)
        .stepBack() // L1

        // Lower lip
        .operate<OPTranslate>(Vec3{ 0.0, -0.075, 0.17 })
        .combine<COMBAdd, SDFCylinder>(AXIS_Y, 0.25, 0.05, skinCol)() // L2
        .operate<OPRoundOff>(0.1)
        .stepBack() // L1

        // Neck
        .combine<COMBAddSmooth, SDFObject>()(0.15) // L2
        .operate<OPTranslate>(Vec3{ 0.0, -0.15, 0.2 })
        .operate<OPRotate>(Vec3::Right, 0.04)
        .combine<COMBAdd, SDFCylinder>(AXIS_Y, 0.25, 0.5, skinCol)() // L3
        .stepBack(2) // L1

        // Nose
        .operate<OPTranslate>(Vec3{ 0.0, 0.4, -0.6 })
        .combine<COMBAddSmooth, SDFCapsule>(AXIS_Z, 0.1, 0.2, skinCol)(0.05) // L2
        .stepBack() // L1

        // Eye ridges
        .operate<OPMirrorX>()
        .operate<OPTranslate>(Vec3{ 0.18, 0.35, 0.2 })
        .operate<OPRotate>(Vec3{ 1.0, 0.0, 0.5 }.getNormal(), 0.35)
        .combine<COMBAddSmooth, SDFTorus>(AXIS_Y, Vec2{ 0.20, 0.015 }, skinCol)(0.1) // L2
        .stepBack(0) // L0

        // Eyes
        .combine<COMBAdd, SDFObject>()() //L1
        .operate<OPMirrorX>()
        .operate<OPTranslate>(Vec3{ 0.21, -0.05, -0.4 })
        .combine<COMBAdd, SDFSphere>(0.23)() // L2

        // Pupils
        .operate<OPTranslate>(Vec3{ 0.0, -0.05, -0.13 })
        .combine<COMBAdd, SDFSphere>(0.1, Color::Black)() // L3
        .stepBack(0) // L0

        //// Ears
        .combine<COMBAddSmooth, SDFObject>()(0.05) //L1
        .operate<OPMirrorX>()
        .operate<OPTranslate>(Vec3{ 0.55, -0.35, 0.0 })
        .operate<OPRotate>(Vec3::Down, 0.12)
        .operate<OPRotate>(Vec3::Forward, 0.25)
        .combine<COMBAdd, SDFTorus>(AXIS_Y, Vec2{ .075, 0.02 }, skinCol)() // L2
        .operate<OPTranslate>(Vec3::Down * 0.02)
        .combine<COMBAddSmooth, SDFCylinder>(AXIS_Y, 0.06, 0.01, skinCol)(0.02)
        .stepBack(0) // L0

        // Shirt
        .combine<COMBAdd, SDFObject>()() //L1
        .operate<OPTranslate>(Vec3{ 0.0, -0.2, 0.33 })
        .combine<COMBAdd, SDFCone>(1.4, 0.2, shirtCol)() // L2
        .operate<OPTranslate>(Vec3{ 0.0, -0.4, -0.1 })
        .combine<COMBSubtract, SDFSphere>()() // L2
        .stepBack() // L2
        .operate<OPTranslate>(Vec3{ 0.0, -1.45, 0.1 })
        .combine<COMBAddSmooth, SDFSphere>(0.7, shirtCol)(0.02) //L3
        .operate<OPTranslate>(Vec3::Down * 0.5)
        .combine<COMBSubtract, SDFBox>(Vec3{ 2.0, 1.0, 2.0 })() // L4
        .stepBack() // L3

        // Left sleeve
        .combine<COMBAddSmooth, SDFObject>()(0.01) // L4
        .operate<OPTranslate>(Vec3{ 0.45, 1.3, 0.0 })
        .operate<OPRotate>(Vec3{ -0.5, 0.0, -0.75 }.getNormal(), 0.25)
        .animate<ANSmoothCycle1D>("rotations", 1.0, 0.035)
        .operate<OPTranslate>(Vec3::Down * 0.2)
        .combine<COMBAdd, SDFEllipsoid>(Vec3{ 0.25, 0.4, 0.25 }, shirtCol)() // L5
        .operate<OPTranslate>(Vec3::Down * 0.25)
        .operate<OPOnion>()
        .combine<COMBSubtract, SDFBox>(Vec3{ 1.0, 0.425, 1.0 })() // L6
        .stepBack(2) // L4

        // Left Arm
        .operate<OPRotate>(Vec3::Down, 0.125)
        .operate<OPTranslate>(Vec3::Down * 0.25)
        .combine<COMBAdd, SDFCapsule>(AXIS_Y, 0.17, 0.5, skinCol)() // L5
        .operate<OPTranslate>(Vec3::Down * 0.25)
        .operate<OPRotate>(Vec3::Forward, 0.375)
        .animate<ANSmoothCycle1D>("rotations", 1.0, 0.1, 0.3)
        .operate<OPTranslate>(Vec3::Up * 0.1)
        .combine<COMBAddSmooth, SDFTaperedCapsule>(AXIS_Y, Vec2{ 0.15, 0.1 }, 0.5, skinCol)(0.05) // L6
        // Left hand
        .operate<OPTranslate>(Vec3{ 0.0, 0.6, 0.0 })
        .combine<COMBAddSmooth, SDFEllipsoid>(Vec3{ 0.13, 0.13, 0.05 }, skinCol)(0.05) // L7
        // Left fingers
        .operate<OPTranslate>(Vec3{ 0.0, 0.12, 0.0 })
        .combine<COMBAddSmooth, SDFTaperedCapsule>(AXIS_Y, Vec2{ 0.035, 0.03 }, 0.14, skinCol)(0.05) // L8
        .operate<OPMirrorX>()
        .operate<OPTranslate>(Vec3{ 0.1, -0.02, 0.0 })
        .combine<COMBAddSmooth, SDFTaperedCapsule>(AXIS_Y, Vec2{ 0.035, 0.03 }, 0.125, skinCol)(0.05) // L9
        .stepBack(3) // L6
        // Left thumb
        .operate<OPTranslate>(Vec3{ -0.2, 0.03, 0.0 })
        .operate<OPRotate>(Vec3::Forward, 0.3)
        .combine<COMBAddSmooth, SDFTaperedCapsule>(AXIS_Y, Vec2{ 0.035, 0.03 }, 0.15, skinCol)(0.05) // L7
        .stepBack(4) // L3

        // Right sleeve
        .combine<COMBAddSmooth, SDFObject>()(0.01) // L4
        .operate<OPTranslate>(Vec3{ -0.45, 1.3, 0.0 })
        .operate<OPRotate>(Vec3{ 0.25, 0.0, 1.0 }.getNormal(), 0.06)
        .operate<OPTranslate>(Vec3::Down * 0.2)
        .combine<COMBAdd, SDFEllipsoid>(Vec3{ 0.25, 0.4, 0.25 }, shirtCol)() // L5
        .operate<OPTranslate>(Vec3::Down * 0.25)
        .operate<OPOnion>()
        .combine<COMBSubtract, SDFBox>(Vec3{ 1.0, 0.425, 1.0 })() // L6
        .stepBack(2) // L4

        // Right Arm
        .operate<OPRotate>(Vec3::Down, 0.125)
        .operate<OPTranslate>(Vec3::Down * 0.25)
        .combine<COMBAdd, SDFCapsule>(AXIS_Y, 0.17, 0.5, skinCol)()
        .operate<OPTranslate>(Vec3::Down * 0.25)
        .operate<OPRotate>(Vec3::Forward, 0.45)
        .operate<OPTranslate>(Vec3::Up * 0.1)
        .combine<COMBAddSmooth, SDFTaperedCapsule>(AXIS_Y, Vec2{ 0.15, 0.1 }, 0.5, skinCol)(0.05)
        // Right hand
        .operate<OPTranslate>(Vec3{ 0.0, 0.6, 0.0 })
        .combine<COMBAddSmooth, SDFEllipsoid>(Vec3{ 0.13, 0.13, 0.05 }, skinCol)(0.05)
        // Right fingers
        .operate<OPTranslate>(Vec3{ 0.0, 0.12, 0.0 })
        .combine<COMBAddSmooth, SDFTaperedCapsule>(AXIS_Y, Vec2{ 0.035, 0.03 }, 0.14, skinCol)(0.05)
        .operate<OPMirrorX>()
        .operate<OPTranslate>(Vec3{ 0.1, -0.02, 0.0 })
        .combine<COMBAddSmooth, SDFTaperedCapsule>(AXIS_Y, Vec2{ 0.035, 0.03 }, 0.125, skinCol)(0.05)
        .stepBack(3)
        // Right thumb
        .operate<OPTranslate>(Vec3{ -0.2, 0.03, 0.0 })
        .operate<OPRotate>(Vec3::Forward, 0.3)
        .combine<COMBAddSmooth, SDFTaperedCapsule>(AXIS_Y, Vec2{ 0.035, 0.03 }, 0.15, skinCol)(0.05)
        .stepBack(0)

        // My Shorts (Eat Them)
        .combine<COMBAdd, SDFObject>()() // L1

        // Body
        .operate<OPTranslate>(Vec3{ 0.0, -1.95, 0.35 })
        .combine<COMBAdd, SDFSphere>(0.68, shortsCol)() // L2
        .operate<OPTranslate>(Vec3::Up * 0.9)
        .combine<COMBSubtract, SDFBox>(Vec3{ 2.0 })() // L3
        .stepBack() // L2

        // Pants
        .operate<OPMirrorX>()
        .operate<OPTranslate>(Vec3{ 0.28, -1.4, 0.0 })
        .combine<COMBAddSmooth, SDFCylinder>(AXIS_Y, 0.3, 0.8, shortsCol)(0.1) // L3
        .operate<OPTranslate>(Vec3::Down * 0.55)
        .operate<OPOnion>()
        .combine<COMBSubtract, SDFBox>(Vec3{ 2.0, 0.5, 2.0 })() // L4
        .stepBack(0) // L0

        // Legs
        .combine<COMBAdd, SDFObject>()() // L1
        .operate<OPMirrorX>()
        .operate<OPTranslate>(Vec3{ 0.28, -3.0, 0.3 })
        .combine<COMBAdd, SDFCylinder>(AXIS_Y, 0.15, 1.0, skinCol)() // L2
        .stepBack(0) // L0

        // Shoes
        .combine<COMBAdd, SDFObject>()() // L1
        .operate<OPMirrorX>()
        .operate<OPTranslate>(Vec3{ .28, -3.75, 0.35 })
        .combine<COMBAdd, SDFEllipsoid>(Vec3{ 0.275, 0.42, 0.7 }, shortsCol)() // L2
        .operate<OPTranslate>(Vec3::Backward * 0.2)
        .combine<COMBIntersect, SDFSphere>()() // L3
        .operate<OPTranslate>(Vec3::Down * 0.25)
        .combine<COMBSubtract, SDFBox>(Vec3{ 2.0, 0.5, 2.0 })() // L4
        .stepBack(2) // L2
        .operate<OPTranslate>(Vec3::Forward * 0.2)
        .combine<COMBAdd, SDFEllipsoid>(Vec3{ 0.323, 0.423, 0.73 })() // L2
        .combine<COMBIntersectSmooth, SDFBox>(Vec3{ 2.0, 0.1, 2.0 })(0.0) // L3
        .stepBack() // L2
        .operate<OPTranslate>(Vec3::Backward * 0.25)
        .combine<COMBIntersect, SDFSphere>(0.6)() // L3
        .stepBack() // L2
        .operate<OPTranslate>(Vec3{ 0.0, 0.425, 0.2 })
        .combine<COMBAdd, SDFTorus>(AXIS_Y, Vec2{ 0.17, .055 })() //L3
        .operate<OPTranslate>(Vec3{ -0.22, -0.2, -0.1 })
        .operate<OPRotate>(Vec3::Forward, 0.06)
        .combine<COMBAdd, SDFEllipsoid>(Vec3{ 0.02, 0.1, 0.1 })() // L4
        .stepBack(0)

        .addLightSource(Vec3{ 500.0, 500.0, -1000.0 }, 1500000.0, Color{ 1.0, 1.0, 0.9 }, 0.8)
    );
}
