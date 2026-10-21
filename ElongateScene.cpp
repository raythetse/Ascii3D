#include "ElongateScene.h"

ElongateScene::ElongateScene()
{
	double lengthAnim = 2.0;
	double length = 0.0;

	add(SDFBuilder{}

		.operate<OPTranslate>(Vec3::Up * 1.5)
		.operate<OPElongate>(AXIS_X, length, 0.0)
		.animate<ANLinearCycle1D>("length", lengthAnim, 1.46, 1.33, 1.24, 1.4)
		.operate<OPRotate>(Vec3::Up)
		.operate<OPElongate>(AXIS_Y, length, 0.0)
		.animate<ANLinearCycle1D>("length", lengthAnim, 1.24, 1.47, 1.13, 1.38)
		.operate<OPRotate>(Vec3::Right)
		.operate<OPElongate>(AXIS_Z, 0.5, 0.0)
		.animate<ANLinearCycle1D>("length", .5, 0.94, 1.1, 1.24, 1.54)
		.operate<OPRotate>(Vec3::Forward)
		.combine<COMBAdd, SDFTorus>(AXIS_Z, Vec2{ 2.0, 0.25 })()

		.addLightSource(Vec3{ 8.0, 10.0, -10.0 }, 200.0, Color::White, 0.25)
	);
}
