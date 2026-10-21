#include "TestScene.h"

TestScene::TestScene()
{
	add(SDFBuilder{}
		.combine<COMBAdd, SDFSphere>(0.25)().color(Color::Blue)
		.stepBack(0)

		.addLightSource(Vec3{ 8.0, 8.0, -5.0 }, 150.0, Color::White, 0.2)
	);
}
