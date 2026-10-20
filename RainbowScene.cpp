#include "RainbowScene.h"

RainbowScene::RainbowScene()
{
	double objSize = 0.22;
	double objOffset = 0.5;
	Vec3 phase{ 0.0, 0.33, 0.66 };
	double cycleTime = 4.0;

	add(SDFBuilder{}

		.operate<OPTranslate>(Vec3{ -1.0, 0.0, 2.0 })
		.operate<OPRotate>(Vec3::Up, 0.05)
		.combine<COMBAdd, SDFPlane>(Vec3::Up)()
		.combine<COMBAdd, SDFObject>()()
		.combine<COMBAdd, SDFHexagon>(OP2D_REVOLVE, objSize, 1.0 + objOffset)().color(Color::MediumGray)
		.animate<ANSmoothCycle3D>("color", cycleTime, 0.5, phase)
		.combine<COMBAdd, SDFHexagon>(OP2D_REVOLVE, objSize, 1.0 + objOffset * 2.0)().color(Color::MediumGray)
		.animate<ANSmoothCycle3D>("color", cycleTime, 0.5, phase + 1.0 / 7.0)
		.combine<COMBAdd, SDFHexagon>(OP2D_REVOLVE, objSize, 1.0 + objOffset * 3.0)().color(Color::MediumGray)
		.animate<ANSmoothCycle3D>("color", cycleTime, 0.5, phase + 2.0 / 7.0)
		.combine<COMBAdd, SDFHexagon>(OP2D_REVOLVE, objSize, 1.0 + objOffset * 4.0)().color(Color::MediumGray)
		.animate<ANSmoothCycle3D>("color", cycleTime, 0.5, phase + 3.0 / 7.0)
		.combine<COMBAdd, SDFHexagon>(OP2D_REVOLVE, objSize, 1.0 + objOffset * 5.0)().color(Color::MediumGray)
		.animate<ANSmoothCycle3D>("color", cycleTime, 0.5, phase + 4.0 / 7.0)
		.combine<COMBAdd, SDFHexagon>(OP2D_REVOLVE, objSize, 1.0 + objOffset * 6.0)().color(Color::MediumGray)
		.animate<ANSmoothCycle3D>("color", cycleTime, 0.5, phase + 5.0 / 7.0)
		.combine<COMBAdd, SDFHexagon>(OP2D_REVOLVE, objSize, 1.0 + objOffset * 7.0)().color(Color::MediumGray)
		.animate<ANSmoothCycle3D>("color", cycleTime, 0.5, phase + 6.0 / 7.0)
		

		.addLightSource(Vec3{ 5.0, cycleTime, -5.0 }, 50.0, Color::White, 0.5)
	);
}
