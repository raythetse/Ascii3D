#include "RoboticArmScene.h"

RoboticArmScene::RoboticArmScene()
{
	Color armColor = Color::Gold;

	add(SDFBuilder{}
		// Robot
		.operate<OPTranslate>(Vec3::Forward * 2.5)
		.combine<COMBAdd, SDFObject>()()
		.operate<OPTranslate>(Vec3::Down * 2.0)

		// Base
		.combine<COMBAdd, SDFObject>()()
		.operate<OPMirrorXZ>()
		.combine<COMBAdd, SDFObject>()()
		.operate<OPTranslate>(Vec3{ 0.9, 1.5, 0.9 })
		.operate<OPRotate>(Vec3::Up, 0.125)
		.combine<COMBAdd, SDFBox>(Vec3{ 0.5, 0.2, 0.5 })().color(armColor)
		.operate<OPRoundOff>(0.1)
		.stepBack(2)

		.operate<OPTranslate>(Vec3::Down * 1.25)
		.operate<OPRotate>(Vec3::Up, 0.125)
		.operate<OPRotate>(Vec3::Right, 0.25)
		.combine<COMBAddSmooth, SDFCylinder>(AXIS_Y, 3.0, 0.1)(0.25).color(armColor)
		.operate<OPRoundOff>(0.1)
		.stepBack(2)
		.operate<OPTranslate>(Vec3::Up * 1.75)
		.combine<COMBAddSmooth, SDFCylinder>(AXIS_Y, 0.5, 0.2)().color(armColor)
		.stepBack()
		.operate<OPTranslate>(Vec3::Down * 3.25)
		.combine<COMBSubtract, SDFBox>(Vec3{ 6.0 })()
		.stepBack(1)

		.combine<COMBAdd, SDFObject>()()
		.operate<OPTranslate>(Vec3::Up * 3.0)
		.combine<COMBAdd, SDFCylinder>(AXIS_Y, 2.0, 0.1)().color(Color::MediumGray)

		// Bolts
		.operate<OPRepeatCircle>(AXIS_Y, 16)
		.operate<OPTranslate>(Vec3{ 1.8, 0.075, 0.0 })
		.combine<COMBAdd, SDFOctagon>(OP2D_EXTRUDE, 0.05, 0.1)().color(Color::LightGray)
		.stepBack(3)

		// Arm
		.combine<COMBAdd, SDFObject>()()

		// First segment
		.operate<OPRotate>(Vec3::Up, 0.24) // T = 0.09, B = 0.39
		.animate<ANLinearCycle1D>("rotations", 0.15, 1.5, 1.5, 1.5, 1.5)
		.combine<COMBAdd, SDFObject>()()
		.operate<OPTranslate>(Vec3::Up * 3.5)
		.combine<COMBAdd, SDFCylinder>(AXIS_Y, 0.45, 0.25)().color(Color::Blue)
		.operate<OPRoundOff>(0.05)
		.operate<OPTranslate>(Vec3::Up * 0.5)
		.combine<COMBAdd, SDFTaperedCylinder>(AXIS_Y, Vec2{ 0.4, 0.35 }, 1.55)().color(armColor)
		.stepBack(2)
		.operate<OPTranslate>(Vec3::Up * 1.5)
		.combine<COMBAddSmooth, SDFObject>()(0.2)
		.operate<OPElongateZ>(0.9)
		.combine<COMBAdd, SDFTorus>(AXIS_Z, Vec2{ 0.35, 0.05 })().color(armColor)
		.stepBack(2)
		.combine<COMBSubtractSmooth, SDFObject>()(0.1)
		.operate<OPTranslate>(Vec3::Up * 0.25)
		.operate<OPElongateY>(0.25)
		.combine<COMBAdd, SDFCylinder>(AXIS_X, 0.33, 1.0)().color(armColor)
		.stepBack(2)

		// Second segment
		.operate<OPRotate>(Vec3::Forward, -0.055) // T = 0.08, B = -0.19 
		.animate<ANLinearCycle1D>("rotations", -0.135, 1.65, 1.35, 1.65, 1.35)
		.combine<COMBAdd, SDFObject>()()
		.combine<COMBAdd, SDFCylinder>(AXIS_Z, 0.29)().color(armColor)
		.operate<OPMirrorZ>()
		.operate<OPTranslate>(Vec3::Forward * 0.5)
		.combine<COMBAddSmooth, SDFCapsule>(AXIS_X, 0.05, 0.45)(0.025).color(Color::Red)
		.combine<COMBAdd, SDFCapsule>(AXIS_Y, 0.05, 0.45)().color(Color::Red)
		.stepBack(3)
		.operate<OPTranslate>(Vec3::Up * 0.75)
		.combine<COMBAddSmooth, SDFCylinder>(AXIS_Y, 0.29, 1.5)(0.1).color(armColor)
		.stepBack(2)
		.operate<OPTranslate>(Vec3::Up * 1.5)
		.combine<COMBAddSmooth, SDFObject>()(0.2)
		.operate<OPElongateZ>(0.9)
		.combine<COMBAdd, SDFTorus>(AXIS_Z, Vec2{ 0.35, 0.05 })().color(armColor)
		.stepBack(2)
		.combine<COMBSubtractSmooth, SDFObject>()(0.1)
		.operate<OPTranslate>(Vec3::Up * 0.25)
		.operate<OPElongateY>(0.25)
		.combine<COMBAdd, SDFCylinder>(AXIS_X, 0.33, 1.0)().color(armColor)
		.stepBack(2)

		// Third Segment
		.operate<OPRotate>(Vec3::Forward, 0.0) // T = 0.11, B = -0.11
		.animate<ANLinearCycle1D>("rotations", -0.11, 1.75, 1.25, 1.75, 1.25)
		.combine<COMBAdd, SDFObject>()()
		.combine<COMBAdd, SDFCylinder>(AXIS_Z, 0.29)().color(armColor)
		.operate<OPMirrorZ>()
		.operate<OPTranslate>(Vec3::Forward * 0.5)
		.combine<COMBAddSmooth, SDFCapsule>(AXIS_X, 0.05, 0.45)(0.025).color(Color::Red)
		.combine<COMBAdd, SDFCapsule>(AXIS_Y, 0.05, 0.45)().color(Color::Red)
		.stepBack(3)
		.operate<OPTranslate>(Vec3::Up * 0.75)
		.combine<COMBAddSmooth, SDFTaperedCylinder>(AXIS_Y, Vec2{ 0.29, 0.15 }, 1.5)(0.1).color(armColor)
		.stepBack(2)
		.operate<OPTranslate>(Vec3::Up * 1.5)
		.combine<COMBAddSmooth, SDFObject>()(0.2)
		.operate<OPElongateZ>(0.5)
		.combine<COMBAdd, SDFTorus>(AXIS_Z, Vec2{ 0.2, 0.05 })().color(armColor)
		.stepBack(2)
		.combine<COMBSubtractSmooth, SDFObject>()(0.1)
		.operate<OPTranslate>(Vec3::Up * 0.175)
		.operate<OPElongateY>(0.25)
		.combine<COMBAdd, SDFCylinder>(AXIS_X, 0.19, 0.6)().color(armColor)
		.stepBack(2)

		// Forth Segment
		.operate<OPRotate>(Vec3::Forward, 0.035) // T = 0.18, B = -0.11
		.animate<ANLinearCycle1D>("rotations", -0.145, 1.9, 1.1, 1.9, 1.1)
		.combine<COMBAdd, SDFObject>()()
		.combine<COMBAdd, SDFCylinder>(AXIS_Z, 0.18, 0.625)().color(armColor)
		.operate<OPMirrorZ>()
		.operate<OPTranslate>(Vec3::Forward * 0.325)
		.combine<COMBAddSmooth, SDFCapsule>(AXIS_X, 0.025, 0.25)(0.025).color(Color::Red)
		.combine<COMBAdd, SDFCapsule>(AXIS_Y, 0.025, 0.25)().color(Color::Red)
		.stepBack(3)
		.operate<OPTranslate>(Vec3::Up * 0.5)
		.combine<COMBAddSmooth, SDFTaperedCylinder>(AXIS_Y, Vec2{ 0.14, 0.10 }, 1.0)(0.1).color(armColor)
		.stepBack(2)
		.operate<OPTranslate>(Vec3::Up * 0.9)
		.combine<COMBAdd, SDFCylinder>(AXIS_Y, 0.15, 0.05)().color(Color::Blue)

		// Grabber Segment
		.combine<COMBAdd, SDFObject>()()
		.operate<OPRotate>(Vec3::Up, 0.15) // T = 0.0, B = 0.15
		.animate<ANLinearCycle1D>("rotations", 2.0, 1.5, 1.5, 2.0, 1.0)
		.operate<OPTranslate>(Vec3::Up * 0.1)
		.combine<COMBAdd, SDFCylinder>(AXIS_Y, 0.15, 0.05)().color(Color::Blue)
		.combine<COMBAdd, SDFBox>(Vec3{ 0.2, 0.2, 0.05 })().color(Color::LightGray)
		.operate<OPMirrorX>()
		.operate<OPTranslate>(Vec3{ 0.1, 0.1, 0.0 })
		.combine<COMBAddSmooth, SDFCylinder>(AXIS_Z, 0.025, 1.0)(0.02).color(Color::LightGray)
		.stepBack(2)
		.operate<OPMirrorZ>()
		.operate<OPTranslate>(Vec3{ 0.0, 0.425, 0.4 }) // Open Pos = 0.475
		.animate<ANLinearCycle3D>("translation", Vec3{ 0.0, 0.0, 0.07 }, 0.25, 2.75, 0.25, 2.75, 1.5)
		.combine<COMBAdd, SDFBox>(Vec3{ 0.25, 0.75, 0.05 })().color(Color::LightGray)
		.operate<OPTranslate>(Vec3{ 0.0, 0.35, 0.0 })
		.combine<COMBAddSmooth, SDFCylinder>(AXIS_X, 0.05, 0.3)(0.02, 20.0).color(Color::Red)
		.stepBack(3)
		// Held Sphere
		.operate<OPTranslate>(Vec3::Up * 0.55)
		.combine<COMBAdd, SDFSphere>(0.3)(false, 3.0, 3.0, 4.5).color(Color::Green)
		.stepBack(0)

		// Ramp
		.combine<COMBAdd, SDFObject>()()
		.operate<OPTranslate>(Vec3::Up * 1.0)
		.combine<COMBAdd, SDFObject>()()
		.operate<OPTranslate>(Vec3::Right * 2.75)
		.combine<COMBAdd, SDFBoxFrame>(Vec3{ 1.0, 3.0, 4.0 })().color(Color::LightGray)
		.operate<OPTranslate>(Vec3::Up * 2.0)
		.operate<OPRotate>(Vec3::Right, 0.065)
		.operate<OPTranslate>(Vec3::Down * 1.5)
		.combine<COMBAdd, SDFObject>()()
		.operate<OPTranslate>(Vec3::Backward * 0.75)
		.operate<OPElongateZ>(5.5)
		.combine<COMBAdd, SDFTorus>(AXIS_Z, Vec2{ 0.45, 0.05 })().color(Color::Brown)
		.stepBack(2)
		.operate<OPTranslate>(Vec3::Up * 1.5)
		.combine<COMBSubtract, SDFBox>(Vec3{ 1.2, 3.0, 8.0 })()
		.stepBack(2)
		.combine<COMBIntersect, SDFBox>(Vec3{ 1.0, 3.0, 4.0 })()
		.stepBack(0)

		// Rolling Sphere
		.combine<COMBAdd, SDFObject>()()
		.operate<OPTranslate>(Vec3{ 2.75, 1.275, -0.1 })
		.animate<ANLinearCycle3D>("translation", Vec3{ 0.0, 1.0, 2.1 }, 2.0, 1.0, 2.0, 1.0, 4.5)
		.combine<COMBAdd, SDFSphere>(0.3)(false, 3.0, 3.0, 1.5).color(Color::Green)
		.stepBack(0)

		// Landing Tray
		.combine<COMBAdd, SDFObject>()()
		.operate<OPTranslate>(Vec3{ 2.75, -0.125, -2.25 })
		.combine<COMBAdd, SDFCylinder>(AXIS_Y, 0.25, 0.75)().color(Color::Brown)
		.operate<OPTranslate>(Vec3::Up * 0.45)
		.combine<COMBSubtractSmooth, SDFSphere>(0.35)(0.1)

		.addLightSource(Vec3{ 10.0, 16.0, -10.0 }, 250.0, Color::White, 0.5)
	);
}
