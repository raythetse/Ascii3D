#include "TestScene.h"

TestScene::TestScene()
{
	add(SDFBuilder{}
		.color(Color::MediumGray)
		.pushOp<COMBAdd, SDFPlane>(Vec3::Up, -0.5)()
		.pushOp<OPTranslate>(Vec3::Up * 0.5)
		.pushOp<COMBAdd, SDFStar>(SDFStar::Extrude, 25, 5.0, 0.2)()
		.pushOp<COMBIntersect, SDFCylinderY>(4.5, 0.5)()
		.pushOp<COMBAdd, SDFCylinderY>(4.25, 1.0)()
		//.modObj<PMOscillateSineV3>("dimensions", Vec3{ 4.0 }, Vec3{ 0.5 }, Vec3{ 0.333333, 0.6666666, 0.0 })
		//.modObj<PMOscillateSineST>("numPoints", 5.0, 20.0)
		//.modObj<PMOscillateSineV2>("point3", Vec2{ 2.0, 2.5 }, Vec2(0.25, 0.125), Vec2(0.25, 0.0))
		.addLightSource(Vec3{ 5.0, 8.0, -5.0 }, 100.0, Color::White)
	);
}
