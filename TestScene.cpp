#include "TestScene.h"

TestScene::TestScene()
{
	add(SDFBuilder{}
		.addLightSource()
		.operate<OPTranslate>(Vec3{ 0.0 })
		.animate<ANSmoothCycle1D>("translation")
		.combine<COMBAdd, SDFSphere>()()
	);
}
