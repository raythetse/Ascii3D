#include "TestScene.h"

TestScene::TestScene()
{
	add(SDFBuilder{}
		.operate<OPTranslate>(Vec3::Up)
		.combine<COMBAdd, SDFObject>()()
		.operate<OPTranslate>(Vec3::Up)
		.animate<ANSmoothCycle3D>("translation", 8.0, Vec3::Right * 3.0)
		.operate<OPRotate>(Vec3::One)
		.animate<ANContinuous1D>("rotations", -0.25)
		.combine<COMBAdd, SDFBox>(1.5)().color(Color::Red)
		.stepBack(0)
		.combine<COMBExcludeSmooth, SDFObject>()()
		.operate<OPTranslate>(Vec3::Forward * 0.75 + Vec3::Up * 2.0)
		.animate<ANSmoothCycle3D>("translation", 7.24, Vec3::Right * 3.0)
		.operate<OPRotate>(Vec3::One)
		.animate<ANContinuous1D>("rotations", 0.2132)
		.combine<COMBAdd, SDFBox>(1.5)().color(Color::Green)
		.stepBack(0)
		.combine<COMBExcludeSmooth, SDFObject>()()
		.operate<OPTranslate>(Vec3::Backward * 0.75)
		.animate<ANSmoothCycle3D>("translation", 6.67, Vec3::Right * 3.0)
		.operate<OPRotate>(Vec3::One)
		.animate<ANContinuous1D>("rotations", 0.3217)
		.combine<COMBAdd, SDFBox>(1.5)().color(Color::Blue)
		.stepBack(0)

		.addLightSource(Vec3{ 8.0, 8.0, -5.0 }, 150.0, Color::White, 0.2)
	);
}
