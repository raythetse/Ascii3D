#include "ColorBlendScene.h"

ColorBlendScene::ColorBlendScene()
{
	add(SDFBuilder{}

		.operate<OPTranslate>(Vec3::Left * 2.0 + Vec3::Up)
		.combine<COMBAdd, SDFObject>()()
		.combine<COMBAdd, SDFSphere>()().color(Color::Blue)
		.operate<OPTranslate>(Vec3::Up)
		.combine<COMBAddSmooth, SDFSphere>()().color(Color::Red)
		.animateComb<ANSmoothCycle1D>("smoothing", 8.0, 0.5)
		.stepBack(0)

		.combine<COMBAdd, SDFObject>()()
		.operate<OPTranslate>(Vec3::Right * 2.0)
		.combine<COMBAdd, SDFSphere>()().color(Color::Green)
		.operate<OPTranslate>(Vec3::Up)
		.combine<COMBAddSmooth, SDFSphere>()(1.0, 0.0).color(Color::Orange)
		.animateComb<ANSmoothCycle1D>("colorBlending", 8.0, 5.0)
		.stepBack(0)

		.combine<COMBAdd, SDFObject>()()
		.operate<OPTranslate>(Vec3::Right * 4.0)
		.combine<COMBAdd, SDFSphere>()().color(Color::Gold)
		.operate<OPTranslate>(Vec3::Up)
		.combine<COMBAddSmooth, SDFSphere>()(1.0, 5.0).color(Color::Magenta)
		.animateComb<ANSmoothCycle1D>("colorShifting", 8.0, 1.0)
		.stepBack(0)

		.addLightSource(Vec3{ 8.0, 10.0, -10.0 }, 200.0, Color::White, 0.25)
	);
}
