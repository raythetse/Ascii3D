#include "ElongateScene.h"

ElongateScene::ElongateScene()
{
	double lengthAnim = 1.0;
	double length = 0.0;

	add(SDFBuilder{}

		.operate<OPTranslate>(Vec3::Up * 1.5)
		.operate<OPElongate>(AXIS_X, length, 0.0)
		.animate<ANLinearCycle1D>("length", lengthAnim, 1.46, 1.846, 1.64, 1.94)
		.operate<OPRotate>(Vec3::Up)
		.operate<OPElongate>(AXIS_Y, length, 0.0)
		.animate<ANLinearCycle1D>("length", lengthAnim, 1.34, 2.02, 1.13, 1.88)
		.operate<OPRotate>(Vec3::Right)
		.operate<OPElongate>(AXIS_Z, length, 0.0)
		.animate<ANLinearCycle1D>("length", lengthAnim, 0.94, 2.34, 1.24, 2.24)
		.operate<OPRotate>(Vec3::Forward)
		.combine<COMBAdd, SDFSphere>(1.0)()

		.addLightSource(Vec3{ 8.0, 10.0, -10.0 }, 200.0, Color::White, 0.25)
	);
}
