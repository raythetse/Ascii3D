#include "TestScene.h"
#include "Enumerations.h"
#include "MathFunctions.h"

TestScene::TestScene()
{
	Color armColor = Color::Gold;

	add(SDFBuilder{}
		// Robot
		.combination<COMBAdd, SDFObject>()()
			.operation<OPTranslate>(Vec3::Down * 2.0)

			// Base
			.combination<COMBAdd, SDFObject>()()
				.operation<OPMirrorXZ>()
				.combination<COMBAdd, SDFObject>()()
					.operation<OPTranslate>(Vec3{ 0.9, 1.5, 0.9 })
					.operation<OPRotate>(Vec3::Up, 0.125)
					.combination<COMBAdd, SDFBox>(Vec3{ 0.5, 0.2, 0.5 })().color(armColor)
						.operation<OPRoundOff>(0.1)
						.stepBack(2)

				.operation<OPTranslate>(Vec3::Down * 1.25)
				.operation<OPRotate>(Vec3::Up, 0.125)
				.operation<OPRotate>(Vec3::Right, 0.25)
				.combination<COMBAddSmooth, SDFCylinder>(AXIS_Y, 3.0, 0.1)(0.25).color(armColor)
					.operation<OPRoundOff>(0.1)
					.stepBack(2)
			.operation<OPTranslate>(Vec3::Up * 1.75)
			.combination<COMBAddSmooth, SDFCylinder>(AXIS_Y, 0.5, 0.2)().color(armColor)
				.stepBack()
			.operation<OPTranslate>(Vec3::Down * 3.25)
			.combination<COMBSubtract, SDFBox>(Vec3{ 6.0 })()
				.stepBack(1)

			.combination<COMBAdd, SDFObject>()()
				.operation<OPTranslate>(Vec3::Up * 3.0)
				.combination<COMBAdd, SDFCylinder>(AXIS_Y, 2.0, 0.1)().color(Color::MediumGray)

					// Bolts
					.operation<OPRepeatCircle>(16)
					.operation<OPTranslate>(Vec3{ 1.8, 0.075, 0.0 })
					.combination<COMBAdd, SDFOctagon>(OP2D_EXTRUDE, 0.05, 0.1)().color(Color::LightGray)
						.stepBack(3)
			
			// Arm
			.combination<COMBAdd, SDFObject>()()

				// First segment
				.operation<OPRotate>(Vec3::Up, 0.0)
				//.animation<ANLinearCycle1D>("rotations", 0.25, 1.0, 0.946, 1.78, 0.248)
				.combination<COMBAdd, SDFObject>()()
					.operation<OPTranslate>(Vec3::Up * 3.5)
					.combination<COMBAdd, SDFCylinder>(AXIS_Y, 0.45, 0.25)().color(Color::Blue)
						.operation<OPRoundOff>(0.05)
						.operation<OPTranslate>(Vec3::Up * 0.5)
						.combination<COMBAdd, SDFTaperedCylinder>(AXIS_Y, Vec2{ 0.4, 0.35 }, 1.55)().color(armColor)
							.stepBack(2)
					.operation<OPTranslate>(Vec3::Up * 1.5)
					.combination<COMBAddSmooth, SDFObject>()(0.2)
						.operation<OPElongateZ>(0.9)
						.combination<COMBAdd, SDFTorus>(AXIS_Z, Vec2{ 0.35, 0.05 })().color(armColor)
							.stepBack(2)
					.combination<COMBSubtractSmooth, SDFObject>()(0.1)
						.operation<OPTranslate>(Vec3::Up * 0.25)
						.operation<OPElongateY>(0.25)
						.combination<COMBAdd, SDFCylinder>(AXIS_X, 0.33, 1.0)().color(armColor)
							.stepBack(2)

					// Second segment
					.operation<OPRotate>(Vec3::Forward, 0.0)
						//.animation<ANLinearCycle1D>("rotations", 0.2, 1.476, 1.34, 0.948, 0.845, 1.5)
						.combination<COMBAdd, SDFObject>()()
							.combination<COMBAdd, SDFCylinder>(AXIS_Z, 0.29)().color(armColor)
								.operation<OPMirrorZ>()
								.operation<OPTranslate>(Vec3::Forward * 0.5)
								.combination<COMBAddSmooth, SDFCapsule>(AXIS_X, 0.05, 0.45)(0.025).color(Color::Red)
									.combination<COMBAdd, SDFCapsule>(AXIS_Y, 0.05, 0.45)().color(Color::Red)
										.stepBack(3)
							.operation<OPTranslate>(Vec3::Up * 0.75)
							.combination<COMBAddSmooth, SDFCylinder>(AXIS_Y, 0.29, 1.5)(0.1).color(armColor)
								.stepBack(2)
						.operation<OPTranslate>(Vec3::Up * 1.5)
						.combination<COMBAddSmooth, SDFObject>()(0.2)
							.operation<OPElongateZ>(0.9)
							.combination<COMBAdd, SDFTorus>(AXIS_Z, Vec2{ 0.35, 0.05 })().color(armColor)
								.stepBack(2)
						.combination<COMBSubtractSmooth, SDFObject>()(0.1)
							.operation<OPTranslate>(Vec3::Up * 0.25)
							.operation<OPElongateY>(0.25)
							.combination<COMBAdd, SDFCylinder>(AXIS_X, 0.33, 1.0)().color(armColor)
								.stepBack(2)

						// Third Segment
						.operation<OPRotate>(Vec3::Forward, 0.0)
							//.animation<ANLinearCycle1D>("rotations", 0.2, 1.64, 0.768, 1.247, 0.347)
							.combination<COMBAdd, SDFObject>()()
								.combination<COMBAdd, SDFCylinder>(AXIS_Z, 0.29)().color(armColor)
									.operation<OPMirrorZ>()
									.operation<OPTranslate>(Vec3::Forward * 0.5)
									.combination<COMBAddSmooth, SDFCapsule>(AXIS_X, 0.05, 0.45)(0.025).color(Color::Red)
										.combination<COMBAdd, SDFCapsule>(AXIS_Y, 0.05, 0.45)().color(Color::Red)
											.stepBack(3)
								.operation<OPTranslate>(Vec3::Up * 0.75)
								.combination<COMBAddSmooth, SDFTaperedCylinder>(AXIS_Y, Vec2{ 0.29, 0.15 }, 1.5)(0.1).color(armColor)
									.stepBack(2)
							.operation<OPTranslate>(Vec3::Up * 1.5)
							.combination<COMBAddSmooth, SDFObject>()(0.2)
								.operation<OPElongateZ>(0.5)
								.combination<COMBAdd, SDFTorus>(AXIS_Z, Vec2{ 0.2, 0.05 })().color(armColor)
									.stepBack(2)
							.combination<COMBSubtractSmooth, SDFObject>()(0.1)
								.operation<OPTranslate>(Vec3::Up * 0.175)
								.operation<OPElongateY>(0.25)
								.combination<COMBAdd, SDFCylinder>(AXIS_X, 0.19, 0.6)().color(armColor)
									.stepBack(2)

							// Forth Segment
							.operation<OPRotate>(Vec3::Forward, 0.0)
							//.animation<ANLinearCycle1D>("rotations", 0.25, 1.24, 0.902, 1.14, 0.566)
							.combination<COMBAdd, SDFObject>()()
								.combination<COMBAdd, SDFCylinder>(AXIS_Z, 0.18, 0.625)().color(armColor)
									.operation<OPMirrorZ>()
									.operation<OPTranslate>(Vec3::Forward * 0.325)
									.combination<COMBAddSmooth, SDFCapsule>(AXIS_X, 0.025, 0.25)(0.025).color(Color::Red)
										.combination<COMBAdd, SDFCapsule>(AXIS_Y, 0.025, 0.25)().color(Color::Red)
											.stepBack(3)
									.operation<OPTranslate>(Vec3::Up * 0.5)
									.combination<COMBAddSmooth, SDFTaperedCylinder>(AXIS_Y, Vec2{ 0.14, 0.10 }, 1.0)(0.1).color(armColor)
										.stepBack(2)
								.operation<OPTranslate>(Vec3::Up * 0.9)
								.combination<COMBAdd, SDFCylinder>(AXIS_Y, 0.15, 0.05)().color(Color::Blue)

								// Grabber Segment
								.combination<COMBAdd, SDFObject>()()
									.operation<OPRotate>(Vec3::Up, 0.0)
									.animation<ANLinearCycle1D>("rotations", 0.21, 0.94, 1.354, 1.24, 0.849)
									.operation<OPTranslate>(Vec3::Up * 0.1)
									.combination<COMBAdd, SDFCylinder>(AXIS_Y, 0.15, 0.05)().color(Color::Blue)
										.combination<COMBAdd, SDFBox>(Vec3{ 0.2, 0.2, 0.05 })().color(Color::LightGray)
											.operation<OPMirrorX>()
											.operation<OPTranslate>(Vec3{ 0.1, 0.1, 0.0 })
											.combination<COMBAddSmooth, SDFCylinder>(AXIS_Z, 0.025, 0.75)(0.02).color(Color::LightGray)
												.stepBack(2)
										.operation<OPMirrorZ>()
										.operation<OPTranslate>(Vec3{ 0.0, 0.425, 0.2 })
										.animation<ANLinearCycle3D>("translation", Vec3{ 0.0, 0.0, 0.125 })
										.combination<COMBAdd, SDFBox>(Vec3{ 0.25, 0.75, 0.05 })().color(Color::LightGray)
											.operation<OPTranslate>(Vec3{ 0.0, 0.35, 0.0 })
											.combination<COMBAddSmooth, SDFCylinder>(AXIS_X, 0.05, 0.3)(0.02, 20.0).color(Color::Red)
												.stepBack(0)
		
		// Ramp
		.combination<COMBAdd, SDFObject>()()
			.operation<OPTranslate>(Vec3::Up * 1.0)
			.combination<COMBAdd, SDFObject>()()
				.operation<OPTranslate>(Vec3::Right * 2.75)
				.combination<COMBAdd, SDFBox>(Vec3{ 1.0, 3.0, 4.0 })()
					.operation<OPTranslate>(Vec3::Up * 2.0)
					.operation<OPRotate>(Vec3::Right, 0.065)
					.combination<COMBSubtract, SDFBox>(Vec3{ 1.1, 3.0, 6.0 })()
						.stepBack(1)
					.operation<OPTranslate>(Vec3::Down * 1.5)
					.combination<COMBSubtract, SDFCylinder>(AXIS_Z, 0.5, 7.0)()
		



		.addLightSource(Vec3{ 10.0, 16.0, -10.0 }, 250.0, Color::White, 0.5)
	);
}
