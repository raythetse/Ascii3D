#include "TestScene.h"
#include "Enumerations.h"

TestScene::TestScene()
{
	add(SDFBuilder{}

		// Robot
		.combination<COMBAdd, SDFObject>()()
			// Base
			.combination<COMBAdd, SDFObject>()()
				.operation<OPMirrorXZ>()
				.combination<COMBAdd, SDFObject>()()
					.operation<OPTranslate>(Vec3{ 0.9, 1.5, 0.9 })
					.operation<OPRotate>(Vec3::Up, 0.125)
					.combination<COMBAdd, SDFBox>(Vec3{ 0.5, 0.2, 0.5 })()
						.operation<OPRoundOff>(0.1)
						.stepBack(2)

				.operation<OPTranslate>(Vec3::Down * 1.25)
				.operation<OPRotate>(Vec3::Up, 0.125)
				.operation<OPRotate>(Vec3::Right, 0.25)
				.combination<COMBAddSmooth, SDFCylinder>(AXIS_Y, 3.0, 0.1)(0.25)
					.operation<OPRoundOff>(0.1)
					.stepBack(2)
			.operation<OPTranslate>(Vec3::Up * 1.75)
			.combination<COMBAddSmooth, SDFCylinder>(AXIS_Y, 0.5, 0.2)()
				.stepBack()
			.operation<OPTranslate>(Vec3::Down * 3.25)
			.combination<COMBSubtract, SDFBox>(Vec3{ 6.0 })()
				.stepBack(1)

			.combination<COMBAdd, SDFObject>()()
				.operation<OPTranslate>(Vec3::Up * 3.0)
				.combination<COMBAdd, SDFCylinder>(AXIS_Y, 2.0, 0.1)()

					.operation<OPMirrorXZ>()
					.operation<OPRotate>(Vec3::Up, 2.0 / 16.0)
					.operation<OPMirrorZ>()
					.operation<OPRotate>(Vec3::Up, 1.0 / 16.0)
					.operation<OPTranslate>(Vec3{ 1.75, 0.075, 0.0 })
					.combination<COMBAdd, SDFOctagon>(OP2D_EXTRUDE, 0.075, 0.175)()
						.stepBack(3)
			
			// Arm
			.combination<COMBAdd, SDFObject>()()
				.operation<OPTranslate>(Vec3::Up * 3.5)
				.combination<COMBAdd, SDFCylinder>(AXIS_Y, 0.45, 0.25)()
					.operation<OPRoundOff>(0.05)
					.operation<OPTranslate>(Vec3::Up * 0.5)
					.combination<COMBAdd, SDFTaperedCylinder>(AXIS_Y, Vec2{ 0.4, 0.35 }, 1.55)()
						.stepBack(2)
				.operation<OPTranslate>(Vec3::Up * 1.5)
				.combination<COMBAddSmooth, SDFObject>()(0.2)
					.operation<OPElongateZ>(0.9)
					.combination<COMBAdd, SDFTorus>(AXIS_Z, Vec2{ 0.35, 0.05 })()
						.stepBack(2)
				.combination<COMBSubtractSmooth, SDFObject>()(0.1)
					.operation<OPTranslate>(Vec3::Up * 0.2)
					.combination<COMBAdd, SDFBox>(Vec3{ 0.8, 0.5, 0.7 })()
						.stepBack(2)

				
				.combination<COMBAdd, SDFCylinder>(AXIS_Z, 0.3)()
					.operation<OPMirrorZ>()
					.operation<OPTranslate>(Vec3::Forward * 0.5)
					.combination<COMBAddSmooth, SDFCapsule>(AXIS_X, 0.05, 0.45)(0.025)
						.combination<COMBAdd, SDFCapsule>(AXIS_Y, 0.05, 0.45)()
							.stepBack(3)
				.operation<OPTranslate>(Vec3::Up * 0.75)
				.combination<COMBAddSmooth, SDFTaperedCylinder>(AXIS_Y, Vec2{ 0.3, 0.2 }, 1.5)(0.1)
				
		


		.addLightSource(Vec3{ 10.0, 16.0, -10.0 }, 250.0, Color::White, 0.5)
	);
}
