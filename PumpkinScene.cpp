#include "PumpkinScene.h"

PumpkinScene::PumpkinScene()
{
	Color pumpkinCol = Color::Orange;
	Vec3 offset{ 0.0, 1.0, 7.0 };

	add(SDFBuilder{}

		.combine<COMBAdd, SDFPlane>(Vec3::Up, -2.7)()
		.stepBack()

		.operate<OPTranslate>(offset)

		// Shell
		.combine<COMBAdd, SDFObject>()()
		.operate<OPRepeatCircle>(AXIS_Y, 16)
		.operate<OPTranslate>(Vec3::Right * 1.0)
		.combine<COMBAdd, SDFTorus>(AXIS_Z, Vec2{ 3.0, 0.85 })().color(pumpkinCol)
		.stepBack(2)
		.combine<COMBSubtractSmooth, SDFSphere>(3.8)()
		.stepBack(1)
		.combine<COMBSubtract, SDFSolidAngle>(6.0, 0.12)()
		.stepBack(0)

		// Mouth
		.combine<COMBSubtract, SDFObject>()()
		.operate<OPTranslate>(Vec3::Backward * 3.5)
		.combine<COMBAdd, SDFObject>()()
		.operate<OPRepeatCircle>(AXIS_Z, 25)
		.operate<OPTranslate>(Vec3::Right * 1.45)
		.combine<COMBAdd, SDFBox>(Vec3{ 0.4, 3.0, 0.25 })()
		.operate<OPTranslate>(Vec3::Right * 0.25)
		.combine<COMBAdd, SDFBox>(Vec3{ 0.4, 3.0, 0.5 })()
		.operate<OPTranslate>(Vec3::Right * 0.3)
		.combine<COMBAdd, SDFBox>(Vec3{ 0.4, 3.0, 0.25 })()
		.stepBack(4)
		.operate<OPElongate>(AXIS_Z, 2.0)
		.operate<OPRotate>(Vec3::Forward, 0.5)
		.combine<COMBIntersect, SDFTorusSegment>(AXIS_Z, Vec2{ 2.0, 1.0 })()
		.stepBack(0)

		// Nose
		.combine<COMBSubtract, SDFObject>()()
		.operate<OPTranslate>(Vec3::Backward * 4.0)
		.operate<OPMirror>()
		.operate<OPTranslate>(Vec3::Right * 0.4)
		.combine<COMBAdd, SDFCylinder>(AXIS_Z, 0.3, 2.0)()
		.stepBack(0)

		// Eyes
		.combine<COMBSubtract, SDFObject>()()
		.operate<OPTranslate>(Vec3{ 0.0, 1.5 , -4.0 })
		.operate<OPMirror>()
		.operate<OPTranslate>(Vec3::Right * 0.35)
		.combine<COMBAdd, SDFCylinder>(AXIS_Z, 1.5, 3.0)()
		.operate<OPRotate>(Vec3::Forward, 0.1)
		.operate<OPTranslate>(Vec3{ 1.7, 0.65, 0.0 })
		.combine<COMBIntersect, SDFBox>(Vec3{ 2.0, 2.0, 3.1 })()
		.stepBack(0)

		// Nose Ring
		.combine<COMBAdd, SDFObject>()()
		.operate<OPTranslate>(Vec3{ 0.0, -0.4, -4.65 })
		.operate<OPRotate>(Vec3::Right, 0.125)
		.combine<COMBAdd, SDFTorus>(AXIS_Y, Vec2{ 0.5, 0.1 })()
		.stepBack(0)

		// Cap
		.combine<COMBAdd, SDFObject>()()
		.operate<OPTranslate>(Vec3{ 0.05, 3.2, -0.05 })
		.operate<OPRotate>(Vec3{ -1.0, 0.5, -0.65 }.getNormal(), -0.02)
		.operate<OPTranslate>(Vec3::Down * 3.0)
		.combine<COMBAdd, SDFObject>()()
		.operate<OPRepeatCircle>(AXIS_Y, 16)
		.operate<OPTranslate>(Vec3::Right * 1.0)
		.combine<COMBAdd, SDFTorus>(AXIS_Z, Vec2{ 3.0, 0.85 })().color(pumpkinCol)
		.stepBack(2)
		.combine<COMBSubtractSmooth, SDFSphere>(3.5)()
		.stepBack(1)
		.combine<COMBIntersect, SDFSolidAngle>(6.0, 0.12)()
		.stepBack(1)
		.operate<OPTranslate>(Vec3::Up * 4.2)
		.combine<COMBAdd, SDFStar>(OP2D_EXTRUDE, 10, 0.2, 0.1, 1.2)().color(Color::Green)
		.stepBack(0)

		.addLightSource(offset + Vec3{ 5.0, 8.0, -5.0 }, 50.0, Color::White, 0.5)
		.addLightSource(offset + Vec3::Up, 70.0, Color::Orange, 0.075)
		.addLightSource(offset + Vec3::Up * 2.0, 70.0, Color::Orange, 0.075)
	);
}
