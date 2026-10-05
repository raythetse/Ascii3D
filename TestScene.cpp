#include "TestScene.h"

TestScene::TestScene()
{
	add(SDFBuilder{}
		.pushOp<COMBAdd, SDFPlane>(Vec3::Up, -0.5)()
		.pushOp<COMBAdd, SDFBoxFrame>(Vec3{ 2.0, 1.0, 2.0 }, 0.1)()
		.pushOp<OPTranslate>(Vec3::Up * 0.5)
		.pushOp<COMBAdd, SDFStar>(SDFObject::Revolve, 6, 0.25, 0.5, 2.0)()
		.addLightSource(Vec3{ 5.0, 8.0, -5.0 }, 100.0, Color::White)
	);
}
