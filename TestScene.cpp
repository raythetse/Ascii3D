#include "TestScene.h"

TestScene::TestScene()
{
	std::vector<std::string> serial{
		{ "OPTranslate translation 0 3 0" },
		{ "COMBAdd" },
		{ "SDFPlane color 0.5 1 0.5 normal 0 1 0 offset - 3.8" }
	};

	using in = std::string;
	add(SDFBuilder{}.loadSerialized(serial)
		.comb<COMBAdd, SDFTriangle>(OP2D_REVOLVE)()
		.modObj<MODWaveSmoothV2>("revolveRotateOffset", 8.0)
		.modObj<MODWaveSmooth>("revolveRotations", 13.789)
		.addLightSource(Vec3{ 5.0, 8.0, -5.0 }, 100.0, Color::White)
	);
}
