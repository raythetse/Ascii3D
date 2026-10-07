#include "SDFObject.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include "Vec2.h"
#include "SDFOperation.h"

double SDFObject::extrude2D(double pDotY, double d2D, double height) const
{
    Vec2 w{ d2D, std::abs(pDotY) - height * 0.5 };
    return std::min(std::max(w.x, w.y), 0.0) + w.maximum(0.0).length();
}

Vec3 SDFObject::revolve2D(Vec3 p, double radius) const
{
    return Vec3(Vec2(p.x, p.y).length() - radius, p.z, p.z);
}

SDFObject::~SDFObject() {}

void SDFObject::update()
{
    for (auto& propMod : m_propMods) {
        propMod->update();
    }
    for (auto& op : m_operations) {
        op->update();
    }
}

SDFOperation* SDFObject::pushOperation(std::unique_ptr<SDFOperation> operation)
{
    auto ptr = operation.get();
    m_operations.push_back(std::move(operation));
    return ptr;
}

void SDFObject::setColor(Color color)
{
    m_color = color;
}

Color SDFObject::getColor()
{
    return m_color;
}

bool SDFObject::getShaderPropModCalls(std::string& propName) const
{
    for (auto& propMod : m_propMods) {
        if (propMod->getShaderCall(propName)) return true;
    }
    return false;
}

void SDFObject::serialize(std::vector<std::string>& strVec, size_t level) const
{
    std::stringstream ss{};
    ss << std::string(level, '\t');
    ss << getClassName();
    ss << " color " << m_color.x << ' ' << m_color.y << ' ' << m_color.z;
    serializeInner(ss);
    strVec.push_back(ss.str());
    ++level;
    for (auto& op : m_operations) {
        op->serialize(strVec, level);
    }
    for (auto& mod : m_propMods) {
        mod->serialize(strVec, level, true);
    }
    strVec.push_back(std::string(level, '\t') + "stepBack");
}

double SDFObject::signedDistance(Vec3 p, Color* colorOut) const
{
    double distance{ signedDistanceInner(p) };
    Color colorIn = m_color;
    for (auto& op : m_operations) {
        op->execute(p, distance, colorIn, colorOut);
        if (colorOut) colorIn = *colorOut;
    }
    if (colorOut && m_operations.size() == 0) {
        *colorOut = m_color;
    }
    return distance;
}

void SDFObject::getShaderCalls(std::stringstream& ss, size_t level, size_t& maxLevel)
{
    ss << "\t// SDFObject //\n";
    std::string propName{ "" };
    ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "color") ? propName : m_color.getShader()) << ";\n";
    if (level > maxLevel) {
        ss << "\tvec3 p" << level << ";\n";
        ss << "\tfloat d" << level << " = FLOAT_MAX;\n";
        ss << "\tvec3 color" << level << ";\n";
        maxLevel = level;
    }
    if (level != 0) {
        ss << "\tp" << level << " = p" << (level - 1) << ";\n";
        ss << "\td" << level << " = FLOAT_MAX;\n";
    }
    ss << "\tcolor" << level << " = pmVec30;\n";
    getShaderObjectCalls(ss, level, maxLevel);
    ss << "\n";

    for (auto& op : m_operations) {
        op->getShaderCalls(ss, level, maxLevel);
    }
}

void SDFObject::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
}

double SDFObject::signedDistanceInner(Vec3 p) const
{
    return std::numeric_limits<double>::max();
}

void SDFPlane::serializeInner(std::stringstream& ss) const
{
    ss << " normal " << m_normal.x << ' ' << m_normal.y << ' ' << m_normal.z;
    ss << " offset " << m_offset;
}

void SDFPlane::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFPlane //\n";
    std::string propName{ "" };
    ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "normal") ? "normalize(" + propName + ")" : m_normal.getShader()) << ";\n";
    ss << "\td" << level << " = dot(p" << level << ", pmVec30) - " << m_offset << ";\n\n";
}

double SDFPlane::signedDistanceInner(Vec3 p) const
{
    return p.dot(m_normal) - m_offset;
}

void SDFPlane::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "normal") {
        propMod->BindToProp(&m_normal);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "offset") {
        propMod->BindToProp(&m_offset);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFBox::serializeInner(std::stringstream& ss) const
{
    ss << " dimensions " << m_dimensions.x << ' ' << m_dimensions.y << ' ' << m_dimensions.z;
}

void SDFBox::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFBox //\n";
    std::string propName{ "" };
    ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "dimensions") ? propName : m_dimensions.getShader()) << ";\n";
    ss << "\td" << level << " = box(p" << level << ", pmVec30);\n\n";
}

double SDFBox::signedDistanceInner(Vec3 p) const
{
    p = p.abs();
    p -= Vec3(m_dimensions) * 0.5;
    return p.maximum(0.0).length() + std::min(std::max(p.x, std::max(p.y, p.z)), 0.0);
}

void SDFBox::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "dimensions") {
        propMod->BindToProp(&m_dimensions);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFBoxFrame::serializeInner(std::stringstream& ss) const
{
    ss << " dimensions " << m_dimensions.x << ' ' << m_dimensions.y << ' ' << m_dimensions.z;
    ss << " edgeThickness " << m_edgeThickness;
}

void SDFBoxFrame::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFBoxFrame //\n";
    std::string propName{ "" };
    ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "dimensions") ? propName : m_dimensions.getShader()) << ";\n";
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "edgeThickness") ? propName : m_dimensions.getShader()) << ";\n";
    ss << "\td" << level << " = boxFrame(p" << level << ", pmVec30, pmFloat0);\n\n";
}

double SDFBoxFrame::signedDistanceInner(Vec3 p) const
{
    p = p.abs() - m_dimensions * 0.5;
    double halfThick = m_edgeThickness * 0.5;
    Vec3 q = (p + halfThick).abs() - halfThick;
    return std::min(std::min(
        Vec3(p.x, q.y, q.z).maximum(0.0).length() + std::min(std::max(p.x, std::max(q.y, q.z)), 0.0),
        Vec3(q.x, p.y, q.z).maximum(0.0).length() + std::min(std::max(q.x, std::max(p.y, q.z)), 0.0)),
        Vec3(q.x, q.y, p.z).maximum(0.0).length() + std::min(std::max(q.x, std::max(q.y, p.z)), 0.0));
}

void SDFBoxFrame::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "dimensions") {
        propMod->BindToProp(&m_dimensions);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "edgeThickness") {
        propMod->BindToProp(&m_edgeThickness);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFSphere::serializeInner(std::stringstream& ss) const
{
    ss << " radius " << m_radius;
}

void SDFSphere::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFSphere //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\td" << level << " = sphere(p" << level << ", pmFloat0);\n\n";
}

double SDFSphere::signedDistanceInner(Vec3 p) const
{
    return p.length() - m_radius;
}

void SDFSphere::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "radius") {
        propMod->BindToProp(&m_radius);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFCrystal::serializeInner(std::stringstream& ss) const
{
    ss << " edgeLength " << m_edgeLength;
}

void SDFCrystal::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFCrystal //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "edgeLength") ? propName : std::to_string(m_edgeLength)) << ";\n";
    ss << "\td" << level << " = crystal(p" << level << ", pmFloat0);\n\n";
}

double SDFCrystal::signedDistanceInner(Vec3 p) const
{
    p = p.abs();
    return (p.x + p.y + p.z - m_edgeLength) * 0.57735027;
}

void SDFCrystal::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "edgeLength") {
        propMod->BindToProp(&m_edgeLength);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFCone::serializeInner(std::stringstream& ss) const
{
    ss << " height " << m_height;
    ss << " sweep " << m_sweep;
}

void SDFCone::getShaderObjectCalls(std::stringstream & ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFCone //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "height") ? propName : std::to_string(m_height)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "sweep") ? propName : std::to_string(m_sweep)) << ";\n";
    ss << "\td" << level << " = cone(p" << level << ", pmFloat0, pmFloat1);\n\n";
}

double SDFCone::signedDistanceInner(Vec3 p) const
{
    Vec2 sincos = MF::fast_sincos(m_sweep * MF::HALFPI);
    Vec2 q = Vec2{ sincos.x / sincos.y, -1.0 } * m_height;
    Vec2 w = Vec2{ Vec2{ p.x, p.z }.length(), p.y };
    Vec2 a = w - q * std::clamp(w.dot(q) / q.dot(q), 0.0, 1.0);
    Vec2 b = w - q * Vec2{ std::clamp(w.x / q.x, 0.0, 1.0), 1.0 };
    double k = q.y >= 0.0 ? 1.0 : -1.0;
    double d = std::min(a.dot(a), b.dot(b));
    double s = std::max(k * (w.x * q.y - w.y * q.x), k * (w.y - q.y));
    return std::sqrt(d) * (s >= 0.0 ? 1.0 : -1.0);
}

void SDFCone::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "height") {
        propMod->BindToProp(&m_height);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "sweep") {
        propMod->BindToProp(&m_sweep);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFSolidAngle::serializeInner(std::stringstream& ss) const
{
    ss << " radius " << m_radius;
    ss << " sweep " << m_sweep;
}

void SDFSolidAngle::getShaderObjectCalls(std::stringstream & ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFSolidAngle //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "sweep") ? propName : std::to_string(m_sweep)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\td" << level << " = solidAngle(p" << level << ", pmFloat0, pmFloat1);\n\n";
}

double SDFSolidAngle::signedDistanceInner(Vec3 p) const
{
    Vec2 q = Vec2{ Vec2{ p.x, p.z }.length(), p.y };
    double l = q.length() - m_radius;
    Vec2 sincos = MF::fast_sincos(m_sweep * MF::PI);
    double m = (q - sincos * std::clamp(q.dot(sincos), 0.0, m_radius)).length();
    return std::max(l, m * ((sincos.y * q.x - sincos.x * q.y) > 0.0 ? 1.0 : -1.0));
}

void SDFSolidAngle::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "radius") {
        propMod->BindToProp(&m_radius);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "sweep") {
        propMod->BindToProp(&m_sweep);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFEllipsoid::serializeInner(std::stringstream& ss) const
{
    ss << " radii " << m_radii.x << ' ' << m_radii.y << ' ' << m_radii.z;
}

void SDFEllipsoid::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFEllipsoid //\n";
    std::string propName{ "" };
    ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "radii") ? propName : m_radii.getShader()) << ";\n";
    ss << "\td" << level << " = ellipsoid(p" << level << ", pmVec30);\n\n";
}

double SDFEllipsoid::signedDistanceInner(Vec3 p) const
{
    double k0 = (p / m_radii).length();
    double k1 = (p / (m_radii * m_radii)).length();
    return k0 * (k0 - 1.0) / k1;
}

void SDFEllipsoid::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "radii") {
        propMod->BindToProp(&m_radii);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFCapsuleX::serializeInner(std::stringstream& ss) const
{
    ss << " radius " << m_radius;
    ss << " bodyLength " << m_bodyLength;
}

void SDFCapsuleX::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFCapsuleX //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "bodyLength") ? propName : std::to_string(m_bodyLength)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\td" << level << " = capsule(p" << level << ", pmFloat0, pmFloat1);\n\n";
}

double SDFCapsuleX::signedDistanceInner(Vec3 p) const
{
    p.x = p.x > 0.0 ? std::max(0.0, p.x - 0.5 * m_bodyLength) : std::min(0.0, p.x + 0.5 * m_bodyLength);
    return p.length() - m_radius;
}

void SDFCapsuleX::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "radius") {
        propMod->BindToProp(&m_radius);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "bodyLength") {
        propMod->BindToProp(&m_bodyLength);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFCapsuleY::serializeInner(std::stringstream& ss) const
{
    ss << " radius " << m_radius;
    ss << " bodyLength " << m_bodyLength;
}

void SDFCapsuleY::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFCapsuleY //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "bodyLength") ? propName : std::to_string(m_bodyLength)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\td" << level << " = capsule(p" << level << ".yxz, pmFloat0, pmFloat1);\n\n";
}

double SDFCapsuleY::signedDistanceInner(Vec3 p) const
{
    p.y = p.y > 0.0 ? std::max(0.0, p.y - 0.5 * m_bodyLength) : std::min(0.0, p.y + 0.5 * m_bodyLength);
    return p.length() - m_radius;
}

void SDFCapsuleY::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "radius") {
        propMod->BindToProp(&m_radius);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "bodyLength") {
        propMod->BindToProp(&m_bodyLength);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFCapsuleZ::serializeInner(std::stringstream& ss) const
{
    ss << " radius " << m_radius;
    ss << " bodyLength " << m_bodyLength;
}

void SDFCapsuleZ::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFCapsuleZ //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "bodyLength") ? propName : std::to_string(m_bodyLength)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\td" << level << " = capsule(p" << level << ".zyx, pmFloat0, pmFloat1);\n\n";
}

double SDFCapsuleZ::signedDistanceInner(Vec3 p) const
{
    p.z = p.z > 0.0 ? std::max(0.0, p.z - 0.5 * m_bodyLength) : std::min(0.0, p.z + 0.5 * m_bodyLength);
    return p.length() - m_radius;
}

void SDFCapsuleZ::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "radius") {
        propMod->BindToProp(&m_radius);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "bodyLength") {
        propMod->BindToProp(&m_bodyLength);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFTaperedCapsule::serializeInner(std::stringstream& ss) const
{
    ss << " bodyLength " << m_bodyLength;
    ss << " radii " << m_radii.x << ' ' << m_radii.y;
}

void SDFTaperedCapsule::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFTaperedCapsule //\n";
    std::string propName{ "" };
    ss << "\tpmVec20 = " << (getShaderPropModCalls(propName = "radii") ? propName : m_radii.getShader()) << ";\n";
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "bodyLength") ? propName : std::to_string(m_bodyLength)) << ";\n";
    char axis = getAxis();
    std::string swizzle = (axis == 'Y' ? ".xyz" : (axis == 'X' ? ".yxz" : ".xzy"));
    ss << "d" << level << " = taperedCapsule(p" << level << swizzle <<", pmVec20, pmFloat0); \n\n";
}

double SDFTaperedCapsule::signedDistanceInner(Vec3 p) const
{
    double b = (m_radii.x - m_radii.y) / m_bodyLength;
    double a = sqrt(1.0 - b * b);
    char axis = getAxis();
    Vec2 q = axis == 'Y' ? Vec2{ Vec2{p.x, p.z}.length(), p.y } : axis == 'X' ? Vec2{ Vec2{p.y, p.z}.length(), p.x } : Vec2{ Vec2{p.x, p.y}.length(), p.z };
    double k = q.dot(Vec2{ -b, a });
    if (k < 0.0) return q.length() - m_radii.x;
    if (k > a * m_bodyLength) return (q - Vec2{ 0.0, m_bodyLength }).length() - m_radii.y;
    return q.dot(Vec2{ a, b }) - m_radii.x;
}

void SDFTaperedCapsule::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "bodyLength") {
        propMod->BindToProp(&m_bodyLength);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "radii") {
        propMod->BindToProp(&m_radii);
        m_propMods.push_back(std::move(propMod));
    }
}

char SDFTaperedCapsuleX::getAxis() const
{
    return 'X';
}

char SDFTaperedCapsuleY::getAxis() const
{
    return 'Y';
}

char SDFTaperedCapsuleZ::getAxis() const
{
    return 'Z';
}

void SDFCylinderX::serializeInner(std::stringstream& ss) const
{
    ss << " sideLength " << m_sideLength;
    ss << " radius " << m_radius;
}

void SDFCylinderX::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFCylinderX //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "sideLength") ? propName : std::to_string(m_sideLength)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\td" << level << " = cylinder(p" << level << ", pmFloat0, pmFloat1);\n\n";
}

double SDFCylinderX::signedDistanceInner(Vec3 p) const
{
    Vec2 yz{ p.y, p.z };
    Vec2 d = Vec2{ yz.length(), p.x }.abs() - Vec2(m_radius, m_sideLength * 0.5);
    return std::min(std::max(d.x, d.y), 0.0) + d.maximum(0.0).length();
}

void SDFCylinderX::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "radius") {
        propMod->BindToProp(&m_radius);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "sideLength") {
        propMod->BindToProp(&m_sideLength);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFCylinderY::serializeInner(std::stringstream& ss) const
{
    ss << " sideLength " << m_sideLength;
    ss << " radius " << m_radius;
}

void SDFCylinderY::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFCylinderY //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "sideLength") ? propName : std::to_string(m_sideLength)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\td" << level << " = cylinder(p" << level << ".yxz, pmFloat0, pmFloat1);\n\n";
}

double SDFCylinderY::signedDistanceInner(Vec3 p) const
{
    Vec2 xz{ p.x, p.z };
    Vec2 d = Vec2{ xz.length(), p.y }.abs() - Vec2(m_radius, m_sideLength * 0.5);
    return std::min(std::max(d.x, d.y), 0.0) + d.maximum(0.0).length();
}

void SDFCylinderY::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "radius") {
        propMod->BindToProp(&m_radius);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "sideLength") {
        propMod->BindToProp(&m_sideLength);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFCylinderZ::serializeInner(std::stringstream& ss) const
{
    ss << " sideLength " << m_sideLength;
    ss << " radius " << m_radius;
}

void SDFCylinderZ::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFCylinderZ //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "sideLength") ? propName : std::to_string(m_sideLength)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\td" << level << " = cylinder(p" << level << ".zyx, pmFloat0, pmFloat1);\n\n";
}

double SDFCylinderZ::signedDistanceInner(Vec3 p) const
{
    Vec2 xy{ p.x, p.y };
    Vec2 d = Vec2{ xy.length(), p.z }.abs() - Vec2(m_radius, m_sideLength * 0.5);
    return std::min(std::max(d.x, d.y), 0.0) + d.maximum(0.0).length();
}

void SDFCylinderZ::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "radius") {
        propMod->BindToProp(&m_radius);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "sideLength") {
        propMod->BindToProp(&m_sideLength);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFRhombus::serializeInner(std::stringstream& ss) const
{
    ss << " dimensions " << m_dimensions.x << ' ' << m_dimensions.y;
    ss << " magnitude " << m_magnitude;
    ss << " operation " << m_operation;
}

void SDFRhombus::getShaderObjectCalls(std::stringstream & ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFRhombus //\n";
    std::string propName{ "" };
    ss << "\tpmVec20 = " << (getShaderPropModCalls(propName = "dimensions") ? propName : m_dimensions.getShader()) << ";\n";
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
    ss << "\td" << level << " = rhombus(p" << level << ", pmVec20, " << m_operation << ", pmFloat0);\n\n";
}

double SDFRhombus::signedDistanceInner(Vec3 p) const
{
    if (m_operation == OP2D_REVOLVE) p = revolve2D(p, m_magnitude);
    Vec2 dim2D{ m_dimensions.x, -m_dimensions.y };
    Vec2 p2D{ p.x, p.z };
    p2D = p2D.abs();
    double h = std::clamp((dim2D.dot(p2D) + dim2D.y * dim2D.y) / dim2D.dot(dim2D), 0.0, 1.0);
    p2D -= dim2D * Vec2{ h, h - 1.0 };
    double d2D = p2D.length() * (p2D.x >= 0.0 ? 1.0 : -1.0);
    return m_operation == OP2D_REVOLVE ? d2D : extrude2D(p.y, d2D, m_magnitude);
}

void SDFRhombus::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "dimensions") {
        propMod->BindToProp(&m_dimensions);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "magnitude") {
        propMod->BindToProp(&m_magnitude);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFEquilateralTriangle::serializeInner(std::stringstream& ss) const
{
    ss << " base " << m_base;
    ss << " magnitude " << m_magnitude;
    ss << " operation " << m_operation;
}

void SDFEquilateralTriangle::getShaderObjectCalls(std::stringstream & ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFEquilateralTriangle //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "base") ? propName : std::to_string(m_base)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
    ss << "\td" << level << " = equilateralTriangle(p" << level << ", pmFloat0, " << m_operation << ", pmFloat1);\n\n";
}

double SDFEquilateralTriangle::signedDistanceInner(Vec3 p) const
{
    if (m_operation == OP2D_REVOLVE) p = revolve2D(p, m_magnitude);
    double halfBase = m_base * 0.5;
    Vec2 p2D{ p.x, p.z - m_base * MF::ROOT3 / 6.0 + halfBase };
    p2D.x = std::abs(p2D.x);
    p2D -= Vec2{ 0.5, 0.5 * MF::ROOT3 } * std::max(p2D.x + MF::ROOT3 * p2D.y, 0.0);
    p2D -= Vec2{ std::clamp(p2D.x, -halfBase, halfBase), -halfBase / MF::ROOT3 };
    double d2D = p2D.length() * (p2D.y < 0.0 ? 1.0 : -1.0);
    return m_operation == OP2D_EXTRUDE ? extrude2D(p.y, d2D, m_magnitude * 0.5) : d2D;
}

void SDFEquilateralTriangle::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "base") {
        propMod->BindToProp(&m_base);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "magnitude") {
        propMod->BindToProp(&m_magnitude);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFIsoscelesTriangle::serializeInner(std::stringstream& ss) const
{
    ss << " dimensions " << m_dimensions.x << ' ' << m_dimensions.y;
    ss << " magnitude " << m_magnitude;
    ss << " operation " << m_operation;
}

void SDFIsoscelesTriangle::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFIsoscelesTriangle //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
    ss << "\tpmVec20 = " << (getShaderPropModCalls(propName = "dimensions") ? propName : m_dimensions.getShader()) << ";\n";
    ss << "\td" << level << " = isoscelesTriangle(p" << level << ", pmVec20, " << m_operation << ", pmFloat0);\n\n";
}

double SDFIsoscelesTriangle::signedDistanceInner(Vec3 p) const
{
    if (m_operation == OP2D_REVOLVE) p = revolve2D(p, m_magnitude);
    Vec2 p2D{ std::abs(p.x), p.z };
    Vec2 a = p2D - m_dimensions * std::clamp(p2D.dot(m_dimensions) / m_dimensions.dot(m_dimensions), 0.0, 1.0);
    Vec2 b = p2D - m_dimensions * Vec2{ std::clamp(p2D.x / m_dimensions.x, 0.0, 1.0), 1.0 };
    double s = m_dimensions.y >= 0.0 ? -1.0 : 1.0;
    Vec2 d = Vec2{ a.dot(a), s * (p2D.x * m_dimensions.y - p2D.y * m_dimensions.x) }.minimum(Vec2{ b.dot(b), s * (p2D.y - m_dimensions.y) });
    double d2D = std::sqrt(d.x) * (d.y >= 0.0 ? -1.0 : 1.0);
    return m_operation == OP2D_EXTRUDE ? extrude2D(p.y, d2D, m_magnitude * 0.5) : d2D;
}

void SDFIsoscelesTriangle::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "dimensions") {
        propMod->BindToProp(&m_dimensions);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "magnitude") {
        propMod->BindToProp(&m_magnitude);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFPieSlice::serializeInner(std::stringstream& ss) const
{
    ss << " sweep " << m_sweep;
    ss << " radius " << m_radius;
    ss << " magnitude " << m_magnitude;
    ss << " operation " << m_operation;
}

void SDFPieSlice::getShaderObjectCalls(std::stringstream & ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFPieSlice //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "sweep") ? propName : std::to_string(m_sweep)) << ";\n";
    ss << "\tpmFloat2 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\td" << level << " = pieSlice(p" << level << ", pmFloat1, pmFloat2, " << m_operation << ", pmFloat0);\n\n";
}

double SDFPieSlice::signedDistanceInner(Vec3 p) const
{
    if (m_operation == OP2D_REVOLVE) p = revolve2D(p, m_magnitude);
    Vec2 p2D{ p.x, p.z };
    p2D.x = std::abs(p2D.x);
    double l = p2D.length() - m_radius;
    Vec2 sincos = MF::fast_sincos(m_sweep * MF::PI);
    double m = (p2D - sincos * std::clamp(p2D.dot(sincos), 0.0, m_radius)).length(); // c=sin/cos of aperture
    double d2D = std::max(l, m * (sincos.y * p2D.x - sincos.x * p2D.y >= 0.0 ? 1.0 : -1.0));
    return m_operation == OP2D_EXTRUDE ? extrude2D(p.y, d2D, m_magnitude * 0.5) : d2D;
}

void SDFPieSlice::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "sweep") {
        propMod->BindToProp(&m_sweep);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "radius") {
        propMod->BindToProp(&m_radius);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "magnitude") {
        propMod->BindToProp(&m_magnitude);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFPentagon::serializeInner(std::stringstream& ss) const
{
    ss << " radius " << m_radius;
    ss << " magnitude " << m_magnitude;
    ss << " operation " << m_operation;
}

void SDFPentagon::getShaderObjectCalls(std::stringstream & ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFPentagon //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\td" << level << " = pentagon(p" << level << ", pmFloat1, " << m_operation << ", pmFloat0);\n\n";
}

double SDFPentagon::signedDistanceInner(Vec3 p) const
{
    if (m_operation == OP2D_REVOLVE) p = revolve2D(p, m_magnitude);
    const Vec3 k{ 0.809016994, 0.587785252, 0.726542528 };
    Vec2 p2D{ std::abs(p.x), p.z };
    p2D -= Vec2{ -k.x, k.y } * std::min(Vec2{ -k.x, k.y }.dot(p2D), 0.0) * 2.0;
    p2D -= Vec2 { k.x, k.y } * std::min(Vec2{ k.x, k.y }.dot(p2D), 0.0) * 2.0;
    p2D -= Vec2{ std::clamp(p2D.x, -m_radius * k.z, m_radius * k.z), m_radius };
    double d2D = p2D.length() * (p2D.y >= 0.0 ? 1.0 : -1.0);
    return m_operation == OP2D_EXTRUDE ? extrude2D(p.y, d2D, m_magnitude * 0.5) : d2D;
}

void SDFPentagon::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "radius") {
        propMod->BindToProp(&m_radius);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "magnitude") {
        propMod->BindToProp(&m_magnitude);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFHexagon::serializeInner(std::stringstream& ss) const
{
    ss << " radius " << m_radius;
    ss << " magnitude " << m_magnitude;
    ss << " operation " << m_operation;
}

void SDFHexagon::getShaderObjectCalls(std::stringstream & ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFHexagon //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\td" << level << " = hexagon(p" << level << ", pmFloat1, " << m_operation << ", pmFloat0);\n\n";
}

double SDFHexagon::signedDistanceInner(Vec3 p) const
{
    if (m_operation == OP2D_REVOLVE) p = revolve2D(p, m_magnitude);
    const Vec3 k{ -0.866025404, 0.5, 0.577350269 };
    Vec2 p2D{ std::abs(p.x), std::abs(p.z) };
    p2D -= Vec2{ k.x, k.y } * std::min(Vec2{ k.x, k.y }.dot(p2D), 0.0) * 2.0;
    p2D -= Vec2{ std::clamp(p2D.x, -k.z * m_radius, k.z * m_radius), m_radius };
    double d2D = p2D.length() * (p2D.y >= 0.0 ? 1.0 : -1.0);
    return m_operation == OP2D_EXTRUDE ? extrude2D(p.y, d2D, m_magnitude * 0.5) : d2D;
}

void SDFHexagon::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "radius") {
        propMod->BindToProp(&m_radius);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "magnitude") {
        propMod->BindToProp(&m_magnitude);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFOctagon::serializeInner(std::stringstream& ss) const
{
    ss << " radius " << m_radius;
    ss << " magnitude " << m_magnitude;
    ss << " operation " << m_operation;
}

void SDFOctagon::getShaderObjectCalls(std::stringstream & ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFOctagon //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\td" << level << " = octagon(p" << level << ", pmFloat1, " << m_operation << ", pmFloat0);\n\n";
}

double SDFOctagon::signedDistanceInner(Vec3 p) const
{
    if (m_operation == OP2D_REVOLVE) p = revolve2D(p, m_magnitude);
    const Vec3 k{ -0.9238795325, 0.3826834323, 0.4142135623 };
    Vec2 p2D{ std::abs(p.x), std::abs(p.z) };
    p2D -= Vec2{ k.x, k.y } * std::min(Vec2{ k.x, k.y }.dot(p2D), 0.0) * 2.0;
    p2D -= Vec2{ -k.x, k.y } * std::min(Vec2{ -k.x, k.y }.dot(p2D), 0.0) * 2.0;
    p2D -= Vec2{ std::clamp(p2D.x, -k.z * m_radius, k.z * m_radius), m_radius };
    double d2D = p2D.length() * (p2D.y >= 0.0 ? 1.0 : -1.0);
    return m_operation == OP2D_EXTRUDE ? extrude2D(p.y, d2D, m_magnitude * 0.5) : d2D;
}

void SDFOctagon::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "radius") {
        propMod->BindToProp(&m_radius);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "magnitude") {
        propMod->BindToProp(&m_magnitude);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFStar::serializeInner(std::stringstream& ss) const
{
    ss << " numPoints " << m_numPoints;
    ss << " radius " << m_radius;
    ss << " ratio " << m_ratio;
    ss << " magnitude " << m_magnitude;
    ss << " operation " << m_operation;
}

void SDFStar::getShaderObjectCalls(std::stringstream & ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFStar //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\tpmFloat2 = " << (getShaderPropModCalls(propName = "ratio") ? propName : std::to_string(m_ratio)) << ";\n";
    ss << "\tpmVec20.x = " << (getShaderPropModCalls(propName = "numPoints") ? propName : std::to_string(m_numPoints)) << ";\n";
    ss << "\td" << level << " = star(p" << level << ", int(pmVec20.x), pmFloat1, pmFloat2, " << m_operation << ", pmFloat0);\n\n";
}

double SDFStar::signedDistanceInner(Vec3 p) const
{
    if (m_operation == OP2D_REVOLVE) p = revolve2D(p, m_magnitude);
    Vec2 asincos = MF::fast_sincos(MF::PI / static_cast<double>(m_numPoints));
    Vec2 acs{ asincos.y, asincos.x };
    Vec2 esincos{ MF::fast_sincos(MF::PI / std::lerp(2.0, static_cast<double>(m_numPoints), m_ratio)) };
    Vec2 ecs{ esincos.y, esincos.x };
    double an{ MF::PI / static_cast<double>(m_numPoints) };

    Vec2 p2D{ abs(p.x), p.z };
    double bn = std::fmod(std::atan2(p2D.x, p2D.y), 2.0 * an) - an;
    Vec2 bnsincos = MF::fast_sincos(bn);
    p2D = Vec2{ bnsincos.y, std::abs(bnsincos.x) } * p2D.length();
    p2D -= acs * m_radius;
    p2D += ecs * std::clamp(-(p2D.dot(ecs)), 0.0, m_radius * acs.y / ecs.y);
    double d2D = p2D.length() * (p2D.x >= 0.0 ? 1.0 : -1.0);
    return m_operation == OP2D_EXTRUDE ? extrude2D(p.y, d2D, m_magnitude * 0.5) : d2D;
}

void SDFStar::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "numPoints") {
        propMod->BindToProp(&m_numPoints);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "radius") {
        propMod->BindToProp(&m_radius);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "ratio") {
        propMod->BindToProp(&m_ratio);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "magnitude") {
        propMod->BindToProp(&m_magnitude);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFTriangle::serializeInner(std::stringstream& ss) const
{
    ss << " point1 " << m_point1.x << ' ' << m_point1.y;
    ss << " point2 " << m_point2.x << ' ' << m_point2.y;
    ss << " point3 " << m_point3.x << ' ' << m_point3.y;
    ss << " magnitude " << m_magnitude;
    ss << " operation " << m_operation;
}

void SDFTriangle::getShaderObjectCalls(std::stringstream & ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFTriangle //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
    ss << "\tpmVec20 = " << (getShaderPropModCalls(propName = "point1") ? propName : m_point1.getShader()) << ";\n";
    ss << "\tpmVec21 = " << (getShaderPropModCalls(propName = "point2") ? propName : m_point2.getShader()) << ";\n";
    ss << "\tpmVec22 = " << (getShaderPropModCalls(propName = "point3") ? propName : m_point3.getShader()) << ";\n";
    ss << "\td" << level << " = triangle(p" << level << ", pmVec20, pmVec21, pmVec22, " << m_operation << ", pmFloat0);\n\n";
}

double SDFTriangle::signedDistanceInner(Vec3 p) const
{
    if (m_operation == OP2D_REVOLVE) p = revolve2D(p, m_magnitude);
    Vec2 p2D{ p.x, p.z };
    Vec2 e0 = m_point2 - m_point1;
    Vec2 e1 = m_point3 - m_point2;
    Vec2 e2 = m_point1 - m_point3;
    Vec2 v0 = p2D - m_point1;
    Vec2 v1 = p2D - m_point2;
    Vec2 v2 = p2D - m_point3;
    Vec2 pq0 = v0 - e0 * std::clamp(v0.dot(e0) / e0.dot(e0), 0.0, 1.0);
    Vec2 pq1 = v1 - e1 * std::clamp(v1.dot(e1) / e1.dot(e1), 0.0, 1.0);
    Vec2 pq2 = v2 - e2 * std::clamp(v2.dot(e2) / e2.dot(e2), 0.0, 1.0);
    double s = (e0.x * e2.y - e0.y * e2.x) >= 0.0 ? 1.0 : -1.0;
    Vec2 d = Vec2{ pq0.dot(pq0), s * (v0.x * e0.y - v0.y * e0.x) }
        .minimum(Vec2{ pq1.dot(pq1), s * (v1.x * e1.y - v1.y * e1.x) })
        .minimum(Vec2{ pq2.dot(pq2), s * (v2.x * e2.y - v2.y * e2.x) });
    double d2D = -std::sqrt(d.x) * (d.y >= 0.0 ? 1.0 : -1.0);
    return m_operation == OP2D_EXTRUDE ? extrude2D(p.y, d2D, m_magnitude * 0.5) : d2D;
}

void SDFTriangle::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "point1") {
        propMod->BindToProp(&m_point1);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "point2") {
        propMod->BindToProp(&m_point2);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "point3") {
        propMod->BindToProp(&m_point3);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "magnitude") {
        propMod->BindToProp(&m_magnitude);
        m_propMods.push_back(std::move(propMod));
    }
}
