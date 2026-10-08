#include "TestScene.h"

TestScene::TestScene()
{
	std::vector<std::string> serial{
		{ "OPTranslate translation 0 3 0" },
		{ "COMBAdd" },
		{ "SDFPlane color 0.5 1 0.5 normal 0 1 0 offset - 3.8" }
	};

	using in = std::string;
	add(SDFBuilder{}
		.op<OPMirrorX>().op<OPRotate>(Vec3::Up, 0.25).op<OPMirrorX>()
		.comb<COMBAdd, SDFStar>(SDFStar::DFLT_OPERATION, 18)()
		.animObj<ANWaveSmooth>("numPoints", 16.0, 16.0)
		.addLightSource(Vec3{ 5.0, 8.0, -5.0 }, 100.0, Color::White)
	);
}
