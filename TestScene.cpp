#include "TestScene.h"

TestScene::TestScene()
{
	add(SDFBuilder{}
		.comb<COMBAdd, SDFTorus>(AXIS_Z)()
		.addLightSource(Vec3{ 5.0, 8.0, -5.0 }, 100.0, Color::White)
	);
}
