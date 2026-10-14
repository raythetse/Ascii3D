#include "TestScene.h"
#include "Enumerations.h"

TestScene::TestScene()
{
	add(SDFBuilder{}
		.startBranch("2x2")
		.combination<COMBAdd, SDFBox>(Vec3{ 0.95 })()
		.operation<OPRoundOff>(0.04)
		.operation<OPMirrorXZ>()
		.operation<OPTranslate>(Vec3{ 0.25, 0.5, 0.25 })
		.combination<COMBAdd, SDFCylinder>(AXIS_Y, 0.15, 0.425)()
		.operation<OPRoundOff>(0.02)
		.endBranch()

		//.startBranch("4x4")
		//.combination<COMBAdd, SDFObject>()()
		//.operation<OPMirrorXZ>()
		//.operation<OPTranslate>(Vec3{ 0.5, 0.0, 0.5 })
		//.insertBranch("2x2")
		//.endBranch()

		.operation<OPTranslate>(Vec3{ 1.0, 0.0, 1.0 })
		.insertBranch("2x2")
		.operation<OPTranslate>(Vec3{ 1.0, 0.0, 1.0 })
		.insertBranch("2x2")
		.addLightSource(Vec3{5.0, 5.0, -3.0}, 60.0, Color::White, 0.2)
	);
}
