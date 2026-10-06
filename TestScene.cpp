#include "TestScene.h"

TestScene::TestScene()
{
	add(SDFBuilder{}
		.color(Color::MediumGray)
		.pushOp<COMBAdd, SDFPlane>(Vec3::Up, -0.5)()
		.modObj<PMOscillateSineV3>("color", Vec3{ 4.0 }, Vec3{ 0.5 }, Vec3{ 0.333333, 0.6666666, 0.0 })
		.pushOp<COMBAdd, SDFBoxFrame>(Vec3{ 2.0, 1.0, 2.0 }, 0.1)()
		.modObj<PMOscillateSineV3>("dimensions", Vec3{ 4.0 }, Vec3{ 0.5 }, Vec3{ 0.333333, 0.6666666, 0.0 })
		.pushOp<OPTranslate>(Vec3::Up * 0.5)
		.pushOp<COMBAdd, SDFStar>(SDFObject::Revolve, 6, 0.25, 0.5, 2.0)()
		.addLightSource(Vec3{ 5.0, 8.0, -5.0 }, 100.0, Color::White)
	);
}
