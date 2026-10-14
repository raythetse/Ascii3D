#include "TestScene.h"
#include "Enumerations.h"

TestScene::TestScene()
{
	add(SDFBuilder{}
		.operation<OPTranslate>()
		.animation<ANLinearCycle3D>("translation", Vec3::Right, 1.0, 1.0, 0.0, 1.0, 0.0)
		.combination<COMBAdd, SDFBoxFrame>(Vec3{ 2.0 })()
		.animation<ANLinearCycle3D>("dimensions", Vec3{ 0.75, 0.614, 0.5148 }, Vec3{ .794, .483, .667 }, Vec3{ .845, .378, .849 }, Vec3{ 1.124, .556, .764 }, Vec3{ .617, .885, .409 }, 0.0)
		.operation<OPTranslate>()
		.animation<ANLinearCycle3D>("translation", Vec3::Right, 1.0, 1.0, 0.0, 1.0, 0.0)
		.combination<COMBAdd, SDFBoxFrame>(Vec3{ 2.0 })()
		.animation<ANLinearCycle3D>("dimensions", Vec3{ 0.75, 0.614, 0.5148 }, Vec3{ .794, .483, .667 }, Vec3{ .845, .378, .849 }, Vec3{ 1.124, .556, .764 }, Vec3{ .617, .885, .409 }, 0.0)
		.addLightSource(Vec3{ 5.0, 8.0, -5.0 }, 100.0, Color::White)
	);
}
