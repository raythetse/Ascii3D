#include "SDFScene.h"
#include "FileHandler.h"
#include "SDFOperationInc.h"
#include "SDFBuilder.h"

const std::string SDFScene::m_shaderLayout{ 
    R"( 
        #version 450
        layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

        struct PixelOutput {
            float r, g, b, l;
        };

        layout(std430, binding = 0) buffer OutputBuffer {
            PixelOutput pixels[];
        };

        layout(push_constant) uniform PushConstants {
            float camPX; float camPY; float camPZ;
            float time;
            float camFX; float camFY; float camFZ;
            float width;
            float camUX; float camUY; float camUZ;
            float height;
            float camRX; float camRY; float camRZ;
            float maxDistance;
            float surfTolerance;
            float pad0;
            float pad1;
            float pad2;
        } push;
    )"
};

const std::string SDFScene::m_shaderFunctions{
    R"(
        float sphere(vec3 p, float radius) {
            return length(p) - radius;
        }
    )"
};

const std::string SDFScene::m_shaderMainBegin{
    R"(
        void main() {
            uint coordX = gl_GlobalInvocationID.x;
            uint coordY = gl_GlobalInvocationID.y;
    
            // 2. Strict bounds check (Never remove this!)
            if (coordX >= uint(push.width) || coordY >= uint(push.height)) return;

            vec2 viewportCoords = vec2(float(coordX) / push.width, float(coordY) / push.height);

            vec3 camPos = vec3(push.camPX, push.camPY, push.camPZ);
            vec3 camForward = vec3(push.camFX, push.camFY, push.camFZ);
            vec3 camRight = vec3(push.camRX, push.camRY, push.camRZ);
            vec3 camUp = vec3(push.camUX, push.camUY, push.camUZ);

            vec3 screenPoint = camPos + camForward;
            screenPoint -= camRight;
            screenPoint += camRight * 2.0 * viewportCoords.x;
            screenPoint += camUp * 0.5;
            screenPoint -= camUp * viewportCoords.y;

            vec3 rayDir = normalize(screenPoint - camPos);

            float rayLength = 0.0;
            float d0 = push.maxDistance;

            uint outIndex = (coordY * uint(push.width)) + coordX;
            pixels[outIndex].l = 1.0;

	        while (rayLength < push.maxDistance) {
                vec3 p0 = camPos + rayDir * rayLength;
    )"
};

const std::string SDFScene::m_shaderMainEnd{
    R"(
                if (d0 < push.surfTolerance) {
                    //pixels[outIndex].r = clamp(length(screenPoint), 0.0, 1.0); // Red fades across left-to-right
                    //pixels[outIndex].g = float(coordY) / push.height;  // Green fades down top-to-bottom
                    //pixels[outIndex].b = push.maxDistance;
                    pixels[outIndex].l = rayLength / push.maxDistance;
                    return;
                }
		        rayLength += d0;
	        }
        }
    )"
};

std::string SDFScene::getSceneShader() const
{
    std::stringstream shaderCallStream{};
    size_t maxLevel{ 0 };
    m_baseObject->getShaderCalls(shaderCallStream, 0, maxLevel);
    return m_shaderLayout + m_shaderFunctions + m_shaderMainBegin + shaderCallStream.str() + m_shaderMainEnd;
}

SDFScene::SDFScene(LoadOption loadOption)
{
    switch (loadOption)
    {
    case SDFScene::Test:
        test();
        break;
    case SDFScene::Cubes:
        cubesDemo();
        break;
    case SDFScene::Elongate:
        elongateDemo();
        break;
    case SDFScene::Frame:
        frameDemo();
        break;
    case SDFScene::Oxygen:
        oxygenDemo();
        break;
    case SDFScene::PropMod:
        propModDemo();
        break;
    case SDFScene::Torus:
        torusDemo();
        break;
    default:
        propModTests();
        break;
    }
}

SDFScene::SDFScene(std::string fileName)
{
    std::vector<std::string> strVec;
    FileHandler::readSDFScene(strVec, fileName);

    SDFBuilder builder{};
    builder.loadSerialized(strVec);

    add(builder);
}

SDFScene::SDFScene(SDFBuilder& builder)
{
    m_baseObject = std::move(builder.getBaseObject());
    m_lights = std::move(builder.getLights());
}

SDFScene::SDFScene(SDFBuilder&& builder)
{
    m_baseObject = std::move(builder.getBaseObject());
    m_lights = std::move(builder.getLights());
}

void SDFScene::add(SDFBuilder& builder)
{
    m_baseObject = std::move(builder.getBaseObject());
    m_lights = std::move(builder.getLights());
}

void SDFScene::add(SDFBuilder&& builder)
{
    m_baseObject = std::move(builder.getBaseObject());
    m_lights = std::move(builder.getLights());
}

void SDFScene::update()
{
    m_baseObject->update();
}

bool SDFScene::serialize(std::vector<std::string>& strVec) const
{
    m_baseObject->serialize(strVec, 0);
    for (auto& light : m_lights) {
        light->serialize(strVec);
    }
    strVec.push_back("end");
    return false;
}

void SDFScene::test()
{
    add(SDFBuilder{}
        .pushOp<COMBAdd, SDFSphere>(0.5, Color::Blue, Vec3::Forward, Vec3::One, 0.5)()
            .pushOp<COMBAdd, SDFSphere>(0.5, Color::Blue, Vec3::Down, Vec3::One, 0.5)()
    );
}

void SDFScene::frameDemo()
{
    add(SDFBuilder{}
        .pushOp<COMBAdd, SDFCylinderY>(0.01, 15.0, Color::SkyBlue, Vec3::Down * 4.0)()
            .stepBack()
        .pushOp<OPRotateContinuous>(Vec3::Up, 0.1)
        .pushOp<OPRotateContinuous>(Vec3::Right, 0.1)
        .pushOp<OPRotateContinuous>(Vec3::Forward, 0.1)
        .pushOp<COMBAdd, SDFBoxFrame>(Vec3{ 2.0 }, 0.1, Color::Gold)()
            .pushOp<OPTranslateSine>(Vec3::Up, 3.0, 2.0)
            .pushOp<OPRotateContinuous>(Vec3::Down, 0.1)
            .pushOp<OPRotateContinuous>(Vec3::Left, 0.1)
            .pushOp<OPRotateContinuous>(Vec3::Backward, 0.1)
                .pushOp<COMBAdd, SDFObject>()()
                    .pushOp<COMBAdd, SDFBox>(Vec3::One, Color::Red, Vec3{ 0.5, 0.5, 0.5 })().stepBack()
                    .pushOp<COMBAdd, SDFBox>(Vec3::One, Color::Orange, Vec3{ 0.5, 0.5, -0.5 })().stepBack()
                    .pushOp<COMBAdd, SDFBox>(Vec3::One, Color::Magenta, Vec3{ 0.5, -0.5, 0.5 })().stepBack()
                    .pushOp<COMBAdd, SDFBox>(Vec3::One, Color::MediumGray, Vec3{ 0.5, -0.5, -0.5 })().stepBack()
                    .pushOp<COMBAdd, SDFBox>(Vec3::One, Color::Green, Vec3{ -0.5, 0.5, 0.5 })().stepBack()
                    .pushOp<COMBAdd, SDFBox>(Vec3::One, Color::Yellow, Vec3{ -0.5, 0.5, -0.5 })().stepBack()
                    .pushOp<COMBAdd, SDFBox>(Vec3::One, Color::Blue, Vec3{ -0.5, -0.5, 0.5 })().stepBack()
                    .pushOp<COMBAdd, SDFBox>(Vec3::One, Color::Violet, Vec3{ -0.5, -0.5, -0.5 })().stepBack()
                    .pushOp<COMBAdd, SDFBoxFrame>(Vec3{ 1.25 }, 0.1, Color::White)()
        .addLightSource(Vec3{ 1.0, 8.0, -1.0 }, 150.0, Color::White)
    );
}

void SDFScene::oxygenDemo()
{
    auto builder = SDFBuilder{};
    
    size_t numElectrons = 8;

    for (size_t i{}; i < numElectrons; ++i) {
        double phase{ i * MF::TAU };
        double dist = static_cast<double>(i) / static_cast<double>(numElectrons);
        dist *= dist * dist;
        dist = 1.5 + dist;
        double rotPerSec = i % 2 ? 0.5 : -0.5 ;
        builder
            .pushOp<COMBAdd, SDFObject>()()
                .pushOp<OPRotateContinuous>(Vec3::One, 0.01, phase)
                .pushOp<OPRotateContinuous>(Vec3::Up, rotPerSec, phase)
                .pushOp<OPTranslate>(Vec3::Right * dist)
                .pushOp<COMBAdd, SDFSphere>(0.1, Color::Yellow)()
                    .stepBack(2);
    }

    builder
        .pushOp<OPRotateContinuous>(Vec3::Up, 0.25)
            .pushOp<COMBAdd, SDFSphere>(0.25, Color::Red)().pushOp<OPTranslate>(Vec3{ 0.224, 0.2468, -0.18 })
                .pushOp<COMBAdd, SDFSphere>(0.25, Color::Blue)().pushOp<OPTranslate>(Vec3{ -0.1287, -0.14864, .167 })
                    .pushOp<COMBAdd, SDFSphere>(0.25, Color::Red)().pushOp<OPTranslate>(Vec3{ 0.187, -0.0458, 0.2 })
                        .pushOp<COMBAdd, SDFSphere>(0.25, Color::Blue)().pushOp<OPTranslate>(Vec3{ -0.054, -0.217, -0.17 })
                            .pushOp<COMBAdd, SDFSphere>(0.25, Color::Red)().pushOp<OPTranslate>(Vec3{ -.1, -.2256, .076 })
                                .pushOp<COMBAdd, SDFSphere>(0.25, Color::Blue)().pushOp<OPTranslate>(Vec3{ 0.0, 0.26, 0.0 })
                                    .pushOp<COMBAdd, SDFSphere>(0.25, Color::Red)().pushOp<OPTranslate>(Vec3{ -0.2, 0.01, 0.0 })
                                        .pushOp<COMBAdd, SDFSphere>(0.25, Color::Blue)().pushOp<OPTranslate>(Vec3{ 0.19, -0.084, -0.02 })
        .addLightSource(Vec3{ 1.0, 4.0, -1.0 }, 1.0, Color::White)
    ;

    add(std::move(builder));
}

void SDFScene::cubesDemo()
{
    double smoothing{ 0.2 };
    double colorBlending{ 0.95 };

    add(SDFBuilder()
        .pushOp<COMBAdd, SDFObject>()()
            .pushOp<OPCubeRollHeight>(1.0, 1.0, 0.5)
            .pushOp<OPTranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
            .pushOp<OPTranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
            .pushOp<OPCubeRoller>(std::vector{ Vec3::Forward, Vec3::Right, Vec3::Backward, Vec3::Left }, 1.0, 0.5)
            .pushOp<COMBAdd, SDFBox>()().color(Color::Red)
                .stepBack(2)
        .pushOp<COMBAddSmooth, SDFObject>()(smoothing, colorBlending)
            .pushOp<OPTranslate>()
            .pushOp<OPCubeRollHeight>(3.0, 1.0, 0.5)
            .pushOp<OPTranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
            .pushOp<OPTranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
            .pushOp<OPCubeRoller>(std::vector{ Vec3::Backward, Vec3::Left, Vec3::Forward, Vec3::Right }, 1.0, 0.5)
            .pushOp<COMBAdd, SDFBox>()().color(Color::Yellow)
                .stepBack(2)
        .pushOp<COMBAddSmooth, SDFObject>()(smoothing, colorBlending)
            .pushOp<OPRotate>()
            .pushOp<OPCubeRollHeight>(1.0, 1.0, 0.5)
            .pushOp<OPTranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
            .pushOp<OPTranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
            .pushOp<OPCubeRoller>(std::vector{ Vec3::Forward, Vec3::Right, Vec3::Backward, Vec3::Left }, 1.0, 0.5)
            .pushOp<COMBAdd, SDFBox>()().color(Color::Blue)
                .stepBack(2)
        .pushOp<COMBAddSmooth, SDFObject>()(smoothing, colorBlending)
            .pushOp<OPTranslate>()
            .pushOp<OPRotate>()
            .pushOp<OPCubeRollHeight>(3.0, 1.0, 0.5)
            .pushOp<OPTranslateLinear>(Vec3::Right * 1.0, 1.0, 1.0)
            .pushOp<OPTranslateLinear>(Vec3::Forward * 1.0, 1.0, 1.0, 1.0)
            .pushOp<OPCubeRoller>(std::vector{ Vec3::Backward, Vec3::Left, Vec3::Forward, Vec3::Right }, 1.0, 0.5)
            .pushOp<COMBAdd, SDFBox>()().color(Color::Green)
    );

    m_lights.push_back(std::make_unique<LightSource>(Vec3{ 1.0, 4.0, -2.0 }, 0.5));
}

void SDFScene::propModDemo()
{
    add(SDFBuilder()
        .pushOp<COMBAdd, SDFSphere>(1.0, Color::Blue)()
            .pushOp<OPTranslate>(Vec3::Left)
            .pushOp<COMBAddSmooth, SDFSphere>(1.0, Color::Red)()
                .bindPropMod<PMOscillateSine>("colorBlending", 1.0, 2.0)
                .pushOp<OPTranslate>(Vec3::Right)
    );

    m_lights.push_back(std::make_unique<LightSource>(Vec3{ 1.0, 4.0, -1.0 }, 0.5));
}

void SDFScene::elongateDemo()
{
    add(SDFBuilder{}
        .pushOp<OPTranslateSine>(Vec3::Up, 0.1, 2.0)
        .pushOp<OPRotateContinuous>(Vec3::Up, 0.125)
        .pushOp<OPElongateXYZ>(Vec3{ 0.5, 1.0, 0.75 })
        .bindPropMod<PMOscillateSineV3>("lengths", Vec3{ 1.0, 0.849, 0.731 }, Vec3{ 0.5, 0.5, 0.5 })
        .pushOp<COMBAdd, SDFCrystal>(1.25)().color(Color::Blue)
            .pushOp<COMBSubtractSmooth, SDFBox>(1.5, Color::Red)(0.5, 25.0, 0.25)
                .stepBack()
            .pushOp<COMBSubtractSmooth, SDFBox>(1.0, Color::Green)(0.1, 50.0, 0.4925)
                .stepBack()
            .pushOp<COMBAdd, SDFCrystal>(0.1, Color::Yellow)()
        .addLightSource(Vec3{ 3.0, 3.0, -3.0 }, 150.0, Color::White)
    );
}

void SDFScene::torusDemo()
{
    double lightPower{ 350.0 };
    double lightDist{ 16.0 };

    add(SDFBuilder{}
        .pushOp<COMBAdd, SDFBox>(50.0, Color::MediumGray)()
        .pushOp<OPInsideOut>()
        .pushOp<OPTranslate>(Vec3{ -2.0, 1.0, 1.0 })
        .pushOp<OPRotateContinuous>(Vec3::Up, 0.242)
        .pushOp<OPRotateContinuous>(Vec3::Right, 0.143)
        .pushOp<COMBAdd, SDFTorus>(Vec2{ 6.0, 1.0 }, Color::White)()
        .stepBack()
        .pushOp<OPTranslate>(Vec3{ -2.0, 1.0, 1.0 })
        .pushOp<OPRotateContinuous>(Vec3::Up, 0.187)
        .pushOp<OPRotateContinuous>(Vec3::Right, 0.231)
        .pushOp<COMBAddSmooth, SDFTorus>(Vec2{ 5.0, 1.25 }, Color::White)(3.0)
        .addLightSource({ -lightDist }, lightPower, Color::Pink)
        .addLightSource({ lightDist }, lightPower, Color::LightBlue)
        .addLightSource({ 0.5 * lightDist, 0.0, -lightDist }, lightPower, Color::LightGreen)
    );
}

void SDFScene::propModTests()
{
    add(SDFBuilder{}
        .pushOp<OPElongateXYZ>()
        .bindPropMod<PMOscillateSineV3>("lengths", Vec3{1.54,2.14,3.12}, Vec3{0.64, 0.75, 0.48})
        .pushOp<COMBAdd, SDFCrystal>()()
        .addLightSource(Vec3{ 3.0, 3.0, -3.0 }, 150.0, Color::White)
    );
}
