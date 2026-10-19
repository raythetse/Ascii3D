#include "PumpkinScene.h"

PumpkinScene::PumpkinScene()
{
	Color pumpkinCol = Color::Orange;
	Vec3 offset{ 0.0, 1.0, 7.0 };

	add(SDFBuilder{}

		.combination<COMBAdd, SDFPlane>(Vec3::Up, -2.7)()
		.stepBack()

		.operation<OPTranslate>(offset)

		// Shell
		.combination<COMBAdd, SDFObject>()()
		.operation<OPRepeatCircle>(AXIS_Y, 16)
		.operation<OPTranslate>(Vec3::Right * 1.0)
		.combination<COMBAdd, SDFTorus>(AXIS_Z, Vec2{ 3.0, 0.85 })().color(pumpkinCol)
		.stepBack(2)
		.combination<COMBSubtractSmooth, SDFSphere>(3.8)()
		.stepBack(1)
		.combination<COMBSubtract, SDFSolidAngle>(6.0, 0.12)()
		.stepBack(0)

		// Mouth
		.combination<COMBSubtract, SDFObject>()()
		.operation<OPTranslate>(Vec3::Backward * 3.5)
		.combination<COMBAdd, SDFObject>()()
		.operation<OPRepeatCircle>(AXIS_Z, 25)
		.operation<OPTranslate>(Vec3::Right * 1.45)
		.combination<COMBAdd, SDFBox>(Vec3{ 0.4, 3.0, 0.25 })()
		.operation<OPTranslate>(Vec3::Right * 0.25)
		.combination<COMBAdd, SDFBox>(Vec3{ 0.4, 3.0, 0.5 })()
		.operation<OPTranslate>(Vec3::Right * 0.3)
		.combination<COMBAdd, SDFBox>(Vec3{ 0.4, 3.0, 0.25 })()
		.stepBack(4)
		.operation<OPElongateZ>(2.0)
		.operation<OPRotate>(Vec3::Forward, 0.5)
		.combination<COMBIntersect, SDFTorusSegment>(AXIS_Z, Vec2{ 2.0, 1.0 })()
		.stepBack(0)

		// Nose
		.combination<COMBSubtract, SDFObject>()()
		.operation<OPTranslate>(Vec3::Backward * 4.0)
		.operation<OPMirrorX>()
		.operation<OPTranslate>(Vec3::Right * 0.4)
		.combination<COMBAdd, SDFCylinder>(AXIS_Z, 0.3, 2.0)()
		.stepBack(0)

		// Eyes
		.combination<COMBSubtract, SDFObject>()()
		.operation<OPTranslate>(Vec3{ 0.0, 1.5 , -4.0 })
		.operation<OPMirrorX>()
		.operation<OPTranslate>(Vec3::Right * 0.35)
		.combination<COMBAdd, SDFCylinder>(AXIS_Z, 1.5, 3.0)()
		.operation<OPRotate>(Vec3::Forward, 0.1)
		.operation<OPTranslate>(Vec3{ 1.7, 0.65, 0.0 })
		.combination<COMBIntersect, SDFBox>(Vec3{ 2.0, 2.0, 3.1 })()
		.stepBack(0)

		// Nose Ring
		.combination<COMBAdd, SDFObject>()()
		.operation<OPTranslate>(Vec3{ 0.0, -0.4, -4.65 })
		.operation<OPRotate>(Vec3::Right, 0.125)
		.combination<COMBAdd, SDFTorus>(AXIS_Y, Vec2{ 0.5, 0.1 })()
		.stepBack(0)

		// Cap
		.combination<COMBAdd, SDFObject>()()
		.operation<OPTranslate>(Vec3{ 0.05, 3.2, -0.05 })
		.operation<OPRotate>(Vec3{ -1.0, 0.5, -0.65 }.getNormal(), -0.02)
		.operation<OPTranslate>(Vec3::Down * 3.0)
		.combination<COMBAdd, SDFObject>()()
		.operation<OPRepeatCircle>(AXIS_Y, 16)
		.operation<OPTranslate>(Vec3::Right * 1.0)
		.combination<COMBAdd, SDFTorus>(AXIS_Z, Vec2{ 3.0, 0.85 })().color(pumpkinCol)
		.stepBack(2)
		.combination<COMBSubtractSmooth, SDFSphere>(3.5)()
		.stepBack(1)
		.combination<COMBIntersect, SDFSolidAngle>(6.0, 0.12)()
		.stepBack(1)
		.operation<OPTranslate>(Vec3::Up * 4.2)
		.combination<COMBAdd, SDFStar>(OP2D_EXTRUDE, 10, 0.2, 0.1, 1.2)().color(Color::Green)
		.stepBack(0)

		.addLightSource(offset + Vec3{ 5.0, 8.0, -5.0 }, 50.0, Color::White, 0.5)
		.addLightSource(offset + Vec3::Up, 70.0, Color::Orange, 0.075)
		.addLightSource(offset + Vec3::Up * 2.0, 70.0, Color::Orange, 0.075)
	);
}
