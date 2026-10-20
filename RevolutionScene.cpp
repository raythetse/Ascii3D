#include "RevolutionScene.h"

RevolutionScene::RevolutionScene()
{
    add(SDFBuilder{}
        
        .combine<COMBAdd, SDFStar>(OP2D_REVOLVE, 6, 1.0, 0.5, 2.0, 1.0, 0.0)()
        .animate<ANContinuous1D>("revolveRotations", 0.25)
        .animate<ANSmoothCycle1D>("ratio", 8.0, 0.45)
        .animate<ANSmoothCycle1D>("radius", 5.647, 0.5)
        .animate<ANSmoothCycle2D>("revolveRotateOffset", Vec2{ 2.174, 3.6781 }, 0.5)

        .addLightSource(Vec3{ 1.0, 10.0, -7.5 }, 200.0, Color::White, 0.15)
    );
}
