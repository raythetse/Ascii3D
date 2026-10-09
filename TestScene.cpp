#include "TestScene.h"
#include "Enumerations.h"

TestScene::TestScene()
{
	add(SDFBuilder{}
		.op<OPTranslate>()
		.animOp<ANLinearCycle3D>("translation", Vec3::Right, 1.0, 1.0, 0.0, 1.0, 0.0)
		.comb<COMBAdd, SDFBoxFrame>(Vec3{2.0})()
		.addLightSource(Vec3{ 5.0, 8.0, -5.0 }, 100.0, Color::White)
	);
}
