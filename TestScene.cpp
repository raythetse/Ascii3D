#include "TestScene.h"

TestScene::TestScene()
{
	add(SDFBuilder{}
		.combine<COMBAdd, SDFObject>()()
			.combine<COMBAdd, SDFSphere>(0.5)()
			.operate<OPOnion>()
			.stepBack(0)
		.operate<OPTranslate>(Vec3{ 0.0, 0.0, -0.5 })
		.combine<COMBAdd, SDFBox>()(true).color(Color::Red)
		.addLightSource(Vec3{ 8.0, 8.0, -5.0 }, 150.0, Color::White, 0.2)
	);
}
