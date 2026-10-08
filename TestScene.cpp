#include "TestScene.h"

TestScene::TestScene()
{
	add(SDFBuilder{}
		.comb<COMBAdd, SDFTriangle>(OP2D_REVOLVE)()
		.modObj<MODWaveSmoothV2>("revolveRotateOffset", 8.0)
		.modObj<MODWaveSmooth>("revolveRotations", 13.789)
		.addLightSource(Vec3{ 5.0, 8.0, -5.0 }, 100.0, Color::White)
	);
}
