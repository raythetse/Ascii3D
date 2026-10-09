#include "TestScene.h"
#include "Enumerations.h"

TestScene::TestScene()
{
	add(SDFBuilder{}
		.op<OPMirrorX>().op<OPRotate>(Vec3::Up, 0.25).op<OPMirrorX>()
		.comb<COMBAdd, SDFBoxFrame>(Vec3{2.0})()
		.animObj<ANLinearCycle3D>("dimensions", 1.0, Vec3{0.5, 0.648, 0.7584}, Vec3{ 0.246, 0.378, 1.24 }, Vec3{ 0.9204, 1.48, 0.784 }, Vec3{ 0.385, 0.48, 0.668 }, 1.0)
		.addLightSource(Vec3{ 5.0, 8.0, -5.0 }, 100.0, Color::White)
	);
}
