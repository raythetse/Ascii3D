#include "TestScene.h"

TestScene::TestScene()
{
	add(SDFBuilder{}
		.color(Color::MediumGray)
		.comb<COMBAdd, SDFPlane>(Vec3::Up, -0.5, Color::Green)(true)
		.op<OPTranslate>(Vec3::Up * 0.5)
		.comb<COMBAdd, SDFSphere>(1.0, Color::Red)(true)
		.op<OPTranslate>(Vec3::Up * 1.0)
		.comb<COMBAdd, SDFSphere>(1.0, Color::MediumGray)(true)
		.modObj<MODOscillateSineV3>("color", Vec3{ 3.0 }, Vec3{ 0.5 }, Vec3{ 0.0, 0.3333333, 0.6666666 })

		//.modObj<PMOscillateSineST>("numPoints", 5.0, 5.0)
		//.modObj<PMOscillateSineV3>("dimensions", Vec3{ 4.0 }, Vec3{ 0.5 }, Vec3{ 0.333333, 0.6666666, 0.0 })
		//.modObj<PMOscillateSineV2>("point3", Vec2{ 2.0, 2.5 }, Vec2(0.25, 0.125), Vec2(0.25, 0.0))
		.addLightSource(Vec3{ 5.0, 8.0, -5.0 }, 100.0, Color::White)
	);
}
