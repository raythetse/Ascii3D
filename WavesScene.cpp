#include "WavesScene.h"

WavesScene::WavesScene()
{
    add(SDFBuilder{}
        
        .operation<OPTranslate>(Vec3{ 0.0, 1.0, 2.0 })
        .combination<COMBAdd, SDFPlane>(Vec3::Up)().color(Color::Blue)
        .stepBack(0)

        // Drop 1
        .combination<COMBAddSmooth, SDFObject>()(0.5)
        .operation<OPTranslate>(Vec3::Right)
        .animation<ANLinearCycle3D>("translation", Vec3{ 0.0, -16.0, 0.0 }, Vec3{ 3.0 }, Vec3{ 0.0 }, Vec3{ 0.0 }, Vec3{ 0.0 }, 0.1)
        .operation<OPRotate>(Vec3::Right, 0.5)
        .combination<COMBAdd, SDFSolidAngle>(0.65, 0.1)().color(Color{ 0.3, 0.3, 1.0 })
        .operation<OPTranslate>(Vec3::Up * 0.625)
        .combination<COMBAddSmooth, SDFSphere>(0.25)().color(Color{ 0.3, 0.3, 1.0 })
        .stepBack(0)

        // Wave 1
        .combination<COMBAddSmooth, SDFObject>()(0.5)
        .operation<OPTranslate>(Vec3::Right)
        .operation<OPTranslate>(Vec3::Zero)
        .animation<ANLinearCycle3D>("translation", Vec3{ 0.0, -1.0, 0.0 }, Vec3{ 3.0 }, Vec3{ 0.0 }, Vec3{ 0.0 }, Vec3{ 0.0 })
        .combination<COMBAdd, SDFTorus>(AXIS_Y)().color(Color{ 0.3, 0.3, 1.0 })
        .animation<ANLinearCycle2D>("radii", Vec2{ 12.0, 0.0 }, Vec2{ 3.0 }, Vec2{ 0.0 }, Vec2{ 0.0 }, Vec2{ 0.0 })
        .stepBack(0)
        
        // Drop 2
        .combination<COMBAddSmooth, SDFObject>()(0.5)
        .operation<OPTranslate>(Vec3::Left)
        .animation<ANLinearCycle3D>("translation", Vec3{ 0.0, -16.0, 0.0 }, Vec3{ 2.317 }, Vec3{ 0.0 }, Vec3{ 0.0 }, Vec3{ 0.0 }, 0.1)
        .operation<OPRotate>(Vec3::Right, 0.5)
        .combination<COMBAdd, SDFSolidAngle>(0.65, 0.1)().color(Color{ 0.3, 0.3, 1.0 })
        .operation<OPTranslate>(Vec3::Up * 0.625)
        .combination<COMBAddSmooth, SDFSphere>(0.25)().color(Color{ 0.3, 0.3, 1.0 })
        .stepBack(0)

        // Wave 2
        .combination<COMBAddSmooth, SDFObject>()(0.5)
        .operation<OPTranslate>(Vec3::Left)
        .animation<ANLinearCycle3D>("translation", Vec3{ 0.0, -1.0, 0.0 }, Vec3{ 2.317 }, Vec3{ 0.0 }, Vec3{ 0.0 }, Vec3{ 0.0 }, Vec3{ 0.0 })
        .combination<COMBAdd, SDFTorus>(AXIS_Y)().color(Color{ 0.3, 0.3, 1.0 })
        .animation<ANLinearCycle2D>("radii", Vec2{ 12.0, 0.0 }, Vec2{ 2.317 }, Vec2{ 0.0 }, Vec2{ 0.0 }, Vec2{ 0.0 }, Vec2{ 0.0 })
        .stepBack(0)


        .addLightSource(Vec3{ 15.0, 15.0, -3.0 }, 350.0, Color::White, 0.5)
    );
}
