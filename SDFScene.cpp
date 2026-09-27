#include "SDFScene.h"
#include "FileHandler.h"
#include "SDFOperationInc.h"
#include "SDFBuilder.h"

const std::string SDFScene::m_shaderLayout{ 
    R"( 
#version 450 core
#extension GL_EXT_control_flow_attributes : enable
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

struct PixelOutput {
    float r, g, b, d, l;
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
const float HALFPI = 1.570796;
const float PI = 3.1415923;
const float TAU = 6.283185;
const float ROOT2 = 1.414213;
const float FLOAT_MAX = 3.402823466e38;
const float NORM_CHECK_OFFSET = 0.001;
const float AMB_LIGHT_POW = 0.05;
const float SHADOW_TOLERANCE = 0.01;

vec3 rotateVec3(vec3 p, vec3 axis, float rotations) {
    vec3 a = normalize(axis);
    float angle = rotations * TAU;
    return p * cos(angle) + cross(a, p) * sin(angle) + a * dot(a, p) * (1.0 - cos(angle));
}

vec3 blendColors(float alpha, vec3 colorA, vec3 colorB, float colorBlending)
{
	alpha *= colorBlending;
	alpha -= 0.5 * (colorBlending - 1.0);
	alpha = clamp(alpha, 0.0, 1.0);
	return mix(colorA, colorB, alpha);
}

float oscillateSine(float initVal, float period, float amplitude, float phase, float time) {
    return initVal + sin(TAU * (time + (phase * period)) / period) * amplitude;
}

float oscillateLinear(float initVal, float period, float amplitude, float phase, float time) {
    float progress = mod(0.25 * period + time + phase * period, period) / period;
	progress -= 0.5;
	progress *= 2.0;
	progress = abs(progress);
	progress -= 0.5;
	progress *= 2.0;
    return initVal + progress * amplitude;
}

vec3 oscillateSineV3(vec3 initVal, vec3 period, vec3 amplitude, vec3 phase, float time) {
    return initVal + sin(TAU * (vec3(time) + (phase * period)) / period) * amplitude;
}

vec3 oscillateLinearV3(vec3 initVal, vec3 period, vec3 amplitude, vec3 phase, float time) {
    vec3 progress = vec3(
        mod(0.25 * period.x + time + phase.x * period.x, period.x) / period.x,
        mod(0.25 * period.y + time + phase.y * period.y, period.y) / period.y,
        mod(0.25 * period.z + time + phase.z * period.z, period.z) / period.z
	);
	progress -= 0.5;
	progress *= 2.0;
	progress = abs(progress);
	progress -= 0.5;
	progress *= 2.0;

	return initVal + progress * amplitude;
}

vec3 translateLinear(vec3 vector, vec2 transitTimes, vec2 endDelays, float phase, float time) {
	float progress = mod(time + phase + transitTimes.x * 0.5, transitTimes.x + transitTimes.y + endDelays.x + endDelays.y);
	vec3 start = vector * -0.5;
	if (progress <= transitTimes.x) {
		progress /= transitTimes.x;
		return mix(start, -start, progress);
	}
	float timeFrame = transitTimes.x + endDelays.x;
	if (progress <= timeFrame) {
		return -start;
	}
	if (progress <= (timeFrame + transitTimes.y)) {
		progress = (progress - transitTimes.x - endDelays.x) / transitTimes.y;
		return mix(-start, start, progress);
	}
	return start;
}

vec3 rotateScan(vec3 p, vec3 axis, vec2 transitTimes, vec2 endDelays, float rotations, float phase, float time) {
    float progress = mod(time + phase, transitTimes.x + transitTimes.y + endDelays.x + endDelays.y);
	if (progress <= transitTimes.x) {
		progress /= transitTimes.x;
        return rotateVec3(p, axis, rotations * progress);
	}
	float timeFrame = transitTimes.x + endDelays.x;
	if (progress <= timeFrame) {
        return rotateVec3(p, axis, rotations);
	}
	timeFrame += transitTimes.y;
	if (progress <= timeFrame) {
		progress = (progress - (transitTimes.x + endDelays.x)) / transitTimes.y;
        return rotateVec3(p, axis, rotations * (1.0 - progress));
	}
	return p;
}

vec3 rotateDelay(vec3 p, vec3 axis, float rotations, float rotateTime, float delayTime, float phase, float time) {
    float cycleTime = rotateTime + delayTime;
	float completeCycles = floor((time + phase) / cycleTime);
	float progress = mod(time + phase, cycleTime);
	completeCycles += min(rotateTime, progress) / rotateTime;
    return rotateVec3(p, axis, completeCycles * rotations);
}
        
float box(vec3 p, vec3 dimensions) {
    p = abs(p) - dimensions * 0.5;
    return length(max(p,0.0)) + min(max(p.x,max(p.y,p.z)),0.0);
}

float boxFrame(vec3 p, vec3 dimensions, float edgeThickness) {
    p = abs(p)-dimensions;
    vec3 q = abs(p+edgeThickness)-edgeThickness;
    return min(min(
        length(max(vec3(p.x,q.y,q.z),0.0))+min(max(p.x,max(q.y,q.z)),0.0),
        length(max(vec3(q.x,p.y,q.z),0.0))+min(max(q.x,max(p.y,q.z)),0.0)),
        length(max(vec3(q.x,q.y,p.z),0.0))+min(max(q.x,max(q.y,p.z)),0.0));
}

float sphere(vec3 p, float radius) {
    return length(p) - radius;
}

float torus(vec3 p, vec2 radii) {
    return length(vec2(length(vec2(p.x, p.z)) - radii.x, p.y)) - radii.y;
}

float capsuleX(vec3 p, float bodyLength, float radius) {
    p.x = p.x > 0.0 ? max(0.0, p.x - 0.5 * bodyLength) : min(0.0, p.x + 0.5 * bodyLength);
    return length(p) - radius;
}

float capsuleY(vec3 p, float bodyLength, float radius) {
    p.y = p.y > 0.0 ? max(0.0, p.y - 0.5 * bodyLength) : min(0.0, p.y + 0.5 * bodyLength);
    return length(p) - radius;
}

float capsuleZ(vec3 p, float bodyLength, float radius) {
    p.z = p.z > 0.0 ? max(0.0, p.z - 0.5 * bodyLength) : min(0.0, p.z + 0.5 * bodyLength);
    return length(p) - radius;
}

float crystal(vec3 p, float edgeLength) {
    p = abs(p);
    return (p.x + p.y + p.z - edgeLength) * 0.57735027;
}

float cylinderX(vec3 p, float bodyLength, float radius) {
    vec2 yz = vec2(p.y, p.z);
    vec2 d = abs(vec2(length(yz), p.x )) - vec2(radius, bodyLength);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float cylinderY(vec3 p, float bodyLength, float radius) {
    vec2 xz = vec2(p.x, p.z);
    vec2 d = abs(vec2(length(xz), p.y )) - vec2(radius, bodyLength);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float cylinderZ(vec3 p, float bodyLength, float radius) {
    vec2 xy = vec2(p.x, p.y);
    vec2 d = abs(vec2(length(xy), p.z )) - vec2(radius, bodyLength);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float chainLink(vec3 p, float sideLength, vec2 radii) {
    p.x = p.x > 0.0 ? max(0.0, p.x - sideLength * 0.5) : min(0.0, p.x + sideLength * 0.5);
    return length(vec2(length(vec2(p.x, p.z)) - radii.x, p.y)) - radii.y;
}

float tube(vec3 p, float height, vec2 radii) {
    p.y = p.y > 0.0 ? max(0.0, p.y - height * 0.5) : min(0.0, p.y + height * 0.5);
    return length(vec2(length(vec2(p.x, p.z)) - radii.x, p.y)) - radii.y;
}

)"
};

const std::string SDFScene::m_shaderMainMainFunction{
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

    uint outIndex = (coordY * uint(push.width)) + coordX;
    pixels[outIndex].l = 0.0;

    bool hit = false;
    vec3 p0;
    float d0;
    vec3 color;

	while (rayLength < push.maxDistance) {
        p0 = camPos + rayDir * rayLength;  // Marched sample point with inherited manipulation
        d0 = push.maxDistance;  // Blendable/selectable distance values
        color = vec3(0.0);

        sampleScene(p0, push.maxDistance, d0, color, push.time);

        if (d0 < push.surfTolerance) {
            pixels[outIndex].d = rayLength / push.maxDistance;
            hit = true;
            break;
        }
		rayLength += d0;
	}

    [[branch]] if (hit) {
        vec3 np = p0 - rayDir * push.surfTolerance;
        vec3 offsetX = vec3(NORM_CHECK_OFFSET, 0.0, 0.0);
        vec3 offsetY = vec3(0.0, NORM_CHECK_OFFSET, 0.0);
        vec3 offsetZ = vec3(0.0, 0.0, NORM_CHECK_OFFSET);

        float d1 = push.maxDistance;
        float d2 = push.maxDistance;
        vec3 colorSample;

        vec3 normal;
        sampleScene(np + offsetX, push.maxDistance, d1, colorSample, push.time);
        sampleScene(np - offsetX, push.maxDistance, d2, colorSample, push.time);
        normal.x = d1 - d2;
        d1 = push.maxDistance;
        d2 = push.maxDistance;
        sampleScene(np + offsetY, push.maxDistance, d1, colorSample, push.time);
        sampleScene(np - offsetY, push.maxDistance, d2, colorSample, push.time);
        normal.y = d1 - d2;
        d1 = push.maxDistance;
        d2 = push.maxDistance;
        sampleScene(np + offsetZ, push.maxDistance, d1, colorSample, push.time);
        sampleScene(np - offsetZ, push.maxDistance, d2, colorSample, push.time);
        normal.z = d1 - d2;

        normal = normalize(normal);

)"
};

std::string SDFScene::buildComputeShader() const
{
    std::stringstream ssSample;
    buildSampleFunction(ssSample);
    std::stringstream ssLight;
    buildLightPasses(ssLight);
    return m_shaderLayout + m_shaderFunctions + ssSample.str() + m_shaderMainMainFunction + ssLight.str() + "\t}\n}\n";
}

void SDFScene::buildSampleFunction(std::stringstream& ss) const
{
    ss << "void sampleScene(in vec3 p0, in float maxDist, out float d0, out vec3 color0, in float time) {\n";
    ss << "\td0 = maxDist;\n";
    ss << "\tfloat blend;\n";
    ss << "\tcolor0 = vec3(0.0);\n\n";
    ss << "\tfloat pmFloat0, pmFloat1, pmFloat2;\n";
    ss << "\tvec2 pmVec20, pmVec21, pmVec22;\n";
    ss << "\tvec3 pmVec30, pmVec31, pmVec32;\n\n";
    size_t maxLevel{ 0 };
    m_baseObject->getShaderCalls(ss, 0, maxLevel);
    ss << "}\n\n";
}

void SDFScene::buildLightPasses(std::stringstream& ss) const
{
    ss << "\t\t// LIGHT SAMPLING //\n";
    ss << "\t\tfloat lightDist;\n";
    ss << "\t\tfloat d0;\n";
    ss << "\t\tfloat rayLength;\n";
    ss << "\t\tvec3 cumulativeColor = vec3(0.0);\n";

    for (size_t i{}; i < m_lights.size(); ++i) {
        ss << "\n\t\t// SAMPLE LIGHT [" << i << "] //\n";
        ss << "\t\trayDir = " << m_lights[i]->position().getShaderVec3() << " - p0;\n";
        ss << "\t\tlightDist = length(rayDir);\n";
        ss << "\t\trayDir = normalize(rayDir);\n";
        ss << "\t\trayLength = push.surfTolerance+0.001;\n";
        ss << "\t\td0 = push.maxDistance;\n";

        ss << "\t\twhile (true) {\n";
        ss << "\t\t\tnp = p0 + rayDir * SHADOW_TOLERANCE + rayLength * rayDir;\n\n";
        ss << "\t\t\tsampleScene(np, lightDist, d0, colorSample, push.time);\n";

        ss << "\t\t\trayLength += d0;\n";
        ss << "\t\t\tif (d0 < push.surfTolerance || rayLength >= lightDist) {\n";
        ss << "\t\t\t\tfloat attenuation = 1.0 / (lightDist * lightDist);\n";
        ss << "\t\t\t\tfloat directLight = (rayLength >= lightDist) ? attenuation * " << m_lights[i]->luminance() << " : 0.0;\n";
        ss << "\t\t\t\tfloat ambLight = " << m_lights[i]->luminance() << " * AMB_LIGHT_POW * attenuation;\n";
        ss << "\t\t\t\tfloat dot = max(0.0, dot(rayDir, normal));\n";
        ss << "\t\t\t\tdirectLight *= dot;\n";
        ss << "\t\t\t\tfloat lightPower = directLight + ambLight;\n";
        ss << "\t\t\t\tcumulativeColor += " << m_lights[i]->color().getShaderVec3() << " * lightPower;\n";
        ss << "\t\t\t\tbreak;\n";
        ss << "\t\t\t}\n";
        ss << "\t\t}\n";
    }
    ss << "\n\t\tcumulativeColor = clamp(cumulativeColor * color, 0.0, 1.0);\n";
    ss << "\t\tcumulativeColor = mix(cumulativeColor, cumulativeColor, 0.2);\n";
    ss << "\t\tpixels[outIndex].r = clamp(cumulativeColor.r, 0.0, 1.0);\n";
    ss << "\t\tpixels[outIndex].g = clamp(cumulativeColor.g, 0.0, 1.0);\n";
    ss << "\t\tpixels[outIndex].b = clamp(cumulativeColor.b, 0.0, 1.0);\n";
    ss << "\t\tpixels[outIndex].l = min(1.0, length(cumulativeColor));\n";
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
        .pushOp<COMBAdd, SDFSphere>(2.0, Color::Blue)()
        .pushOp<OPOnion>(0.25)
        .pushOp<OPOnion>(0.05)
        .pushOp<COMBSubtractSmooth, SDFBox>(5.0, Color::Red, Vec3::Up*3.0)(0.5, 1.0, 0.5)
        .addLightSource(Vec3{ 1.0, 8.0, -1.0 }, 150.0, Color::White)
    );
}

void SDFScene::frameDemo()
{
    add(SDFBuilder{}
        .pushOp<COMBAdd, SDFCylinderY>(15.0, 0.01, Color::SkyBlue, Vec3::Down * 4.0)()
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
        .addLightSource(Vec3{ 1.0, 6.0, -2.0 }, 100.0, Color::White)
    ;

    add(std::move(builder));
}

void SDFScene::cubesDemo()
{
    double smoothing{ 0.25 };
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
                .stepBack(2)
    );

    m_lights.push_back(std::make_unique<LightSource>(Vec3{ 2.0, 6.0, -2.0 }, 100.0));
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
