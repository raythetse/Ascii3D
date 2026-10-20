#include "WavesScene.h"

WavesScene::WavesScene()
{
    add(SDFBuilder{}
        
        .operate<OPTranslate>(Vec3{ 0.0, 1.0, 2.0 })
        .combine<COMBAdd, SDFPlane>(Vec3::Up)().color(Color::Blue)
        .stepBack(0)

        // Drop 1
        .combine<COMBAddSmooth, SDFObject>()(0.5)
        .operate<OPTranslate>(Vec3::Right)
        .animate<ANLinearCycle3D>("translation", Vec3{ 0.0, -16.0, 0.0 }, Vec3{ 3.0 }, Vec3{ 0.0 }, Vec3{ 0.0 }, Vec3{ 0.0 }, 0.1)
        .operate<OPRotate>(Vec3::Right, 0.5)
        .combine<COMBAdd, SDFSolidAngle>(0.65, 0.1)().color(Color{ 0.3, 0.3, 1.0 })
        .operate<OPTranslate>(Vec3::Up * 0.625)
        .combine<COMBAddSmooth, SDFSphere>(0.25)().color(Color{ 0.3, 0.3, 1.0 })
        .stepBack(0)

        // Wave 1
        .combine<COMBAddSmooth, SDFObject>()(0.5)
        .operate<OPTranslate>(Vec3::Right)
        .operate<OPTranslate>(Vec3::Zero)
        .animate<ANLinearCycle3D>("translation", Vec3{ 0.0, -1.0, 0.0 }, Vec3{ 3.0 }, Vec3{ 0.0 }, Vec3{ 0.0 }, Vec3{ 0.0 })
        .combine<COMBAdd, SDFTorus>(AXIS_Y)().color(Color{ 0.3, 0.3, 1.0 })
        .animate<ANLinearCycle2D>("radii", Vec2{ 12.0, 0.0 }, Vec2{ 3.0 }, Vec2{ 0.0 }, Vec2{ 0.0 }, Vec2{ 0.0 })
        .stepBack(0)
        
        // Drop 2
        .combine<COMBAddSmooth, SDFObject>()(0.5)
        .operate<OPTranslate>(Vec3::Left)
        .animate<ANLinearCycle3D>("translation", Vec3{ 0.0, -16.0, 0.0 }, Vec3{ 2.317 }, Vec3{ 0.0 }, Vec3{ 0.0 }, Vec3{ 0.0 }, 0.1)
        .operate<OPRotate>(Vec3::Right, 0.5)
        .combine<COMBAdd, SDFSolidAngle>(0.65, 0.1)().color(Color{ 0.3, 0.3, 1.0 })
        .operate<OPTranslate>(Vec3::Up * 0.625)
        .combine<COMBAddSmooth, SDFSphere>(0.25)().color(Color{ 0.3, 0.3, 1.0 })
        .stepBack(0)

        // Wave 2
        .combine<COMBAddSmooth, SDFObject>()(0.5)
        .operate<OPTranslate>(Vec3::Left)
        .animate<ANLinearCycle3D>("translation", Vec3{ 0.0, -1.0, 0.0 }, Vec3{ 2.317 }, Vec3{ 0.0 }, Vec3{ 0.0 }, Vec3{ 0.0 }, Vec3{ 0.0 })
        .combine<COMBAdd, SDFTorus>(AXIS_Y)().color(Color{ 0.3, 0.3, 1.0 })
        .animate<ANLinearCycle2D>("radii", Vec2{ 12.0, 0.0 }, Vec2{ 2.317 }, Vec2{ 0.0 }, Vec2{ 0.0 }, Vec2{ 0.0 }, Vec2{ 0.0 })
        .stepBack(0)


        .addLightSource(Vec3{ 15.0, 15.0, -3.0 }, 350.0, Color::White, 0.5)
    );
}
