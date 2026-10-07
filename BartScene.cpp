#include "BartScene.h"
#include "SDFBuilder.h"

BartScene::BartScene()
{
    Vec3 skinCol = Vec3{ 234.0, 191.0, 4.0 } / 255.0;
    Vec3 shirtCol = Vec3{ 241.0, 78.0, 40.0 } / 255.0;
    Vec3 shortsCol = Vec3{ 0.0, 157.0, 220.0 } / 255.0;

    add(SDFBuilder{}
        .comb<COMBAdd, SDFPlane>(Vec3::Up, -3.8, Color::LightGreen)() // L1
        .stepBack() // L0
        // Head
        .comb<COMBAdd, SDFObject>()() // L1

        // Skull
        .comb<COMBAdd, SDFCylinderY>(0.46, 1.2, skinCol)() // L2
        .op<OPRoundOff>(0.05)

        // Hair
        .op<OPMirrorXZ>()
        .op<OPRotate>(Vec3::Up, -8.0 / 64.0)
        .op<OPMirrorX>()
        .op<OPRotate>(Vec3::Up, -4.0 / 64.0)
        .op<OPMirrorX>()
        .op<OPRotate>(Vec3::Up, -2.0 / 64.0)
        .op<OPMirrorX>()
        .op<OPRotate>(Vec3::Up, -1.0 / 64.0)
        .op<OPTranslate>(Vec3::Forward * 0.51)
        .op<OPTranslate>(Vec3::Up * 0.75)
        .op<OPRotate>(Vec3::Right, -0.06)
        .comb<COMBAddSmooth, SDFCone>(0.155, 0.045, skinCol)(0.01) // L3
        .op<OPRoundOff>(0.01)
        .stepBack(2) // L1

        // Upper lip contour
        .op<OPTranslate>(Vec3{ 0.0, -0.6, -0.12 })
        .comb<COMBAddSmooth, SDFCylinderY>(0.35, 0.0, skinCol)(0.15) // L2
        .op<OPRoundOff>(0.05)
        .stepBack() // L1

        // Lower lip
        .op<OPTranslate>(Vec3{ 0.0, -0.075, 0.17 })
        .comb<COMBAdd, SDFCylinderY>(0.25, 0.05, skinCol)() // L2
        .op<OPRoundOff>(0.1)
        .stepBack() // L1

        // Neck
        .comb<COMBAddSmooth, SDFObject>()(0.15) // L2
        .op<OPTranslate>(Vec3{ 0.0, -0.15, 0.2 })
        .op<OPRotate>(Vec3::Right, 0.04)
        .comb<COMBAdd, SDFCylinderY>(0.25, 0.5, skinCol)() // L3
        .stepBack(2) // L1

        // Nose
        .op<OPTranslate>(Vec3{ 0.0, 0.4, -0.6 })
        .comb<COMBAddSmooth, SDFCapsuleZ>(0.1, 0.2, skinCol)(0.05) // L2
        .stepBack() // L1

        // Eye ridges
        .op<OPMirrorX>()
        .op<OPTranslate>(Vec3{ 0.18, 0.35, 0.2 })
        .op<OPRotate>(Vec3{ 1.0, 0.0, 0.5 }.getNormal(), 0.35)
        .comb<COMBAddSmooth, SDFTorusXZ>(Vec2{ 0.20, 0.015 }, skinCol)(0.1) // L2
        .stepBack(0) // L0

        // Eyes
        .comb<COMBAdd, SDFObject>()() //L1
        .op<OPMirrorX>()
        .op<OPTranslate>(Vec3{ 0.21, -0.05, -0.4 })
        .comb<COMBAdd, SDFSphere>(0.23)() // L2

        // Pupils
        .op<OPTranslate>(Vec3{ 0.0, -0.05, -0.13 })
        .comb<COMBAdd, SDFSphere>(0.1, Color::Black)() // L3
        .stepBack(0) // L0

        //// Ears
        .comb<COMBAddSmooth, SDFObject>()(0.05) //L1
        .op<OPMirrorX>()
        .op<OPTranslate>(Vec3{ 0.55, -0.35, 0.0 })
        .op<OPRotate>(Vec3::Down, 0.12)
        .op<OPRotate>(Vec3::Forward, 0.25)
        .comb<COMBAdd, SDFTorusXZ>(Vec2{ .075, 0.02 }, skinCol)() // L2
        .op<OPTranslate>(Vec3::Down * 0.02)
        .comb<COMBAddSmooth, SDFCylinderY>(0.06, 0.01, skinCol)(0.02)
        .stepBack(0) // L0

        // Shirt
        .comb<COMBAdd, SDFObject>()() //L1
        .op<OPTranslate>(Vec3{ 0.0, -0.2, 0.33 })
        .comb<COMBAdd, SDFCone>(1.4, 0.055, shirtCol)() // L2
        .op<OPTranslate>(Vec3{ 0.0, -0.4, -0.1 })
        .comb<COMBSubtract, SDFSphere>()() // L2
        .stepBack() // L2
        .op<OPTranslate>(Vec3{ 0.0, -1.45, 0.1 })
        .comb<COMBAddSmooth, SDFSphere>(0.7, shirtCol)(0.02) //L3
        .op<OPTranslate>(Vec3::Down * 0.5)
        .comb<COMBSubtract, SDFBox>(Vec3{ 2.0, 1.0, 2.0 })() // L4
        .stepBack() // L3

        // Left sleeve
        .comb<COMBAddSmooth, SDFObject>()(0.01) // L4
        .op<OPTranslate>(Vec3{ 0.45, 1.3, 0.0 })
        .op<OPRotate>(Vec3{ -0.5, 0.0, -0.75 }.getNormal(), 0.25)
        .modOp<MODOscillateSine>("rotations", 1.0, 0.035)
        .op<OPTranslate>(Vec3::Down * 0.2)
        .comb<COMBAdd, SDFEllipsoid>(Vec3{ 0.25, 0.4, 0.25 }, shirtCol)() // L5
        .op<OPTranslate>(Vec3::Down * 0.25)
        .op<OPOnion>()
        .comb<COMBSubtract, SDFBox>(Vec3{ 1.0, 0.425, 1.0 })() // L6
        .stepBack(2) // L4

        // Left Arm
        .op<OPRotate>(Vec3::Down, 0.125)
        .op<OPTranslate>(Vec3::Down * 0.25)
        .comb<COMBAdd, SDFCapsuleY>(0.17, 0.5, skinCol)() // L5
        .op<OPTranslate>(Vec3::Down * 0.25)
        .op<OPRotate>(Vec3::Forward, 0.375)
        .modOp<MODOscillateSine>("rotations", 1.0, 0.1, 0.3)
        .op<OPTranslate>(Vec3::Up * 0.1)
        .comb<COMBAddSmooth, SDFTaperedCapsule>(Vec2{ 0.15, 0.1 }, 0.5, skinCol)(0.05) // L6
        // Left hand
        .op<OPTranslate>(Vec3{ 0.0, 0.6, 0.0 })
        .comb<COMBAddSmooth, SDFEllipsoid>(Vec3{ 0.13, 0.13, 0.05 }, skinCol)(0.05) // L7
        // Left fingers
        .op<OPTranslate>(Vec3{ 0.0, 0.12, 0.0 })
        .comb<COMBAddSmooth, SDFTaperedCapsule>(Vec2{ 0.035, 0.03 }, 0.14, skinCol)(0.05) // L8
        .op<OPMirrorX>()
        .op<OPTranslate>(Vec3{ 0.1, -0.02, 0.0 })
        .comb<COMBAddSmooth, SDFTaperedCapsule>(Vec2{ 0.035, 0.03 }, 0.125, skinCol)(0.05) // L9
        .stepBack(3) // L6
        // Left thumb
        .op<OPTranslate>(Vec3{ -0.2, 0.03, 0.0 })
        .op<OPRotate>(Vec3::Forward, 0.3)
        .comb<COMBAddSmooth, SDFTaperedCapsule>(Vec2{ 0.035, 0.03 }, 0.15, skinCol)(0.05) // L7
        .stepBack(4) // L3

        // Right sleeve
        .comb<COMBAddSmooth, SDFObject>()(0.01) // L4
        .op<OPTranslate>(Vec3{ -0.45, 1.3, 0.0 })
        .op<OPRotate>(Vec3{ 0.25, 0.0, 1.0 }.getNormal(), 0.06)
        .op<OPTranslate>(Vec3::Down * 0.2)
        .comb<COMBAdd, SDFEllipsoid>(Vec3{ 0.25, 0.4, 0.25 }, shirtCol)() // L5
        .op<OPTranslate>(Vec3::Down * 0.25)
        .op<OPOnion>()
        .comb<COMBSubtract, SDFBox>(Vec3{ 1.0, 0.425, 1.0 })() // L6
        .stepBack(2) // L4

        // Right Arm
        .op<OPRotate>(Vec3::Down, 0.125)
        .op<OPTranslate>(Vec3::Down * 0.25)
        .comb<COMBAdd, SDFCapsuleY>(0.17, 0.5, skinCol)()
        .op<OPTranslate>(Vec3::Down * 0.25)
        .op<OPRotate>(Vec3::Forward, 0.45)
        .op<OPTranslate>(Vec3::Up * 0.1)
        .comb<COMBAddSmooth, SDFTaperedCapsule>(Vec2{ 0.15, 0.1 }, 0.5, skinCol)(0.05)
        // Right hand
        .op<OPTranslate>(Vec3{ 0.0, 0.6, 0.0 })
        .comb<COMBAddSmooth, SDFEllipsoid>(Vec3{ 0.13, 0.13, 0.05 }, skinCol)(0.05)
        // Right fingers
        .op<OPTranslate>(Vec3{ 0.0, 0.12, 0.0 })
        .comb<COMBAddSmooth, SDFTaperedCapsule>(Vec2{ 0.035, 0.03 }, 0.14, skinCol)(0.05)
        .op<OPMirrorX>()
        .op<OPTranslate>(Vec3{ 0.1, -0.02, 0.0 })
        .comb<COMBAddSmooth, SDFTaperedCapsule>(Vec2{ 0.035, 0.03 }, 0.125, skinCol)(0.05)
        .stepBack(3)
        // Right thumb
        .op<OPTranslate>(Vec3{ -0.2, 0.03, 0.0 })
        .op<OPRotate>(Vec3::Forward, 0.3)
        .comb<COMBAddSmooth, SDFTaperedCapsule>(Vec2{ 0.035, 0.03 }, 0.15, skinCol)(0.05)
        .stepBack(0)

        // My Shorts (Eat Them)
        .comb<COMBAdd, SDFObject>()() // L1

        // Body
        .op<OPTranslate>(Vec3{ 0.0, -1.95, 0.35 })
        .comb<COMBAdd, SDFSphere>(0.68, shortsCol)() // L2
        .op<OPTranslate>(Vec3::Up * 0.9)
        .comb<COMBSubtract, SDFBox>(Vec3{ 2.0 })() // L3
        .stepBack() // L2

        // Pants
        .op<OPMirrorX>()
        .op<OPTranslate>(Vec3{ 0.28, -1.4, 0.0 })
        .comb<COMBAddSmooth, SDFCylinderY>(0.3, 0.8, shortsCol)(0.1) // L3
        .op<OPTranslate>(Vec3::Down * 0.55)
        .op<OPOnion>()
        .comb<COMBSubtract, SDFBox>(Vec3{ 2.0, 0.5, 2.0 })() // L4
        .stepBack(0) // L0

        // Legs
        .comb<COMBAdd, SDFObject>()() // L1
        .op<OPMirrorX>()
        .op<OPTranslate>(Vec3{ 0.28, -3.0, 0.3 })
        .comb<COMBAdd, SDFCylinderY>(0.15, 1.0, skinCol)() // L2
        .stepBack(0) // L0

        // Shoes
        .comb<COMBAdd, SDFObject>()() // L1
        .op<OPMirrorX>()
        .op<OPTranslate>(Vec3{ .28, -3.75, 0.35 })
        .comb<COMBAdd, SDFEllipsoid>(Vec3{ 0.275, 0.42, 0.7 }, shortsCol)() // L2
        .op<OPTranslate>(Vec3::Backward * 0.2)
        .comb<COMBIntersect, SDFSphere>()() // L3
        .op<OPTranslate>(Vec3::Down * 0.25)
        .comb<COMBSubtract, SDFBox>(Vec3{ 2.0, 0.5, 2.0 })() // L4
        .stepBack(2) // L2
        .op<OPTranslate>(Vec3::Forward * 0.2)
        .comb<COMBAdd, SDFEllipsoid>(Vec3{ 0.323, 0.423, 0.73 })() // L2
        .comb<COMBIntersectSmooth, SDFBox>(Vec3{ 2.0, 0.1, 2.0 })(0.0) // L3
        .stepBack() // L2
        .op<OPTranslate>(Vec3::Backward * 0.25)
        .comb<COMBIntersect, SDFSphere>(0.6)() // L3
        .stepBack() // L2
        .op<OPTranslate>(Vec3{ 0.0, 0.425, 0.2 })
        .comb<COMBAdd, SDFTorusXZ>(Vec2{ 0.17, .055 })() //L3
        .op<OPTranslate>(Vec3{ -0.22, -0.2, -0.1 })
        .op<OPRotate>(Vec3::Forward, 0.06)
        .comb<COMBAdd, SDFEllipsoid>(Vec3{ 0.02, 0.1, 0.1 })() // L4
        .stepBack(0)

        .addLightSource(Vec3{ 500.0, 500.0, -1000.0 }, 1500000.0, Color{ 1.0, 1.0, 0.9 }, 0.8)
    );
}
