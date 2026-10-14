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
    for (auto& an : m_animations) {
        an->update();
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

bool SDFObject::getShaderAnimationCalls(std::string& propName) const
{
    for (auto& an : m_animations) {
        if (an->getShaderCall(propName)) return true;
    }
    return false;
}

void SDFObject::resetAnimations()
{
    for (auto& op : m_operations) {
        op->resetAnimations();
    }
    for (auto& an : m_animations) {
        an->resetProperty();
    }
}

void SDFObject::serialize(std::vector<std::string>& strVec, size_t level) const
{
    std::stringstream ss{};
    ss << std::string(level, '\t');
    ss << getClassName();
    ss << " color " << m_color;
    serializeInner(ss);
    strVec.push_back(ss.str());
    ++level;
    for (auto& an : m_animations) {
        an->serialize(strVec, level);
    }
    for (auto& op : m_operations) {
        op->serialize(strVec, level);
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
    ss << "\tpmVec30 = " << (getShaderAnimationCalls(propName = "color") ? propName : m_color.getShader()) << ";\n";
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

void SDFObject::bindAnimation(std::unique_ptr<Animation> animation)
{
    if (animation->propName() == "color") {
        animation->BindToProperty(&m_color);
        m_animations.push_back(std::move(animation));
    }
}

double SDFObject::signedDistanceInner(Vec3 p) const
{
    return std::numeric_limits<double>::max();
}

void SDFPlane::serializeInner(std::stringstream& ss) const
{
    ss << " normal " << m_normal;
    ss << " offset " << m_offset;
}

void SDFPlane::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFPlane //\n";
    std::string propName{ "" };
    ss << "\tpmVec30 = " << (getShaderAnimationCalls(propName = "normal") ? "normalize(" + propName + ")" : m_normal.getShader()) << ";\n";
    ss << "\td" << level << " = dot(p" << level << ", pmVec30) - " << m_offset << ";\n\n";
}

double SDFPlane::signedDistanceInner(Vec3 p) const
{
    return p.dot(m_normal) - m_offset;
}

void SDFPlane::bindAnimation(std::unique_ptr<Animation> animation)
{
    if (animation->propName() == "color") {
        animation->BindToProperty(&m_color);
        m_animations.push_back(std::move(animation));
    }
    else if (animation->propName() == "normal") {
        animation->BindToProperty(&m_normal);
        m_animations.push_back(std::move(animation));
    }
    else if (animation->propName() == "offset") {
        animation->BindToProperty(&m_offset);
        m_animations.push_back(std::move(animation));
    }
}

void SDFBox::serializeInner(std::stringstream& ss) const
{
    ss << " dimensions " << m_dimensions;
}

void SDFBox::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFBox //\n";
    std::string propName{ "" };
    ss << "\tpmVec30 = " << (getShaderAnimationCalls(propName = "dimensions") ? propName : m_dimensions.getShader()) << ";\n";
    ss << "\td" << level << " = box(p" << level << ", pmVec30);\n\n";
}

double SDFBox::signedDistanceInner(Vec3 p) const
{
    p = p.abs();
    p -= Vec3(m_dimensions) * 0.5;
    return p.maximum(0.0).length() + std::min(std::max(p.x, std::max(p.y, p.z)), 0.0);
}

void SDFBox::bindAnimation(std::unique_ptr<Animation> animation)
{
    if (animation->propName() == "color") {
        animation->BindToProperty(&m_color);
        m_animations.push_back(std::move(animation));
    }
    else if (animation->propName() == "dimensions") {
        animation->BindToProperty(&m_dimensions);
        m_animations.push_back(std::move(animation));
    }
}

void SDFBoxFrame::serializeInner(std::stringstream& ss) const
{
    ss << " dimensions " << m_dimensions;
    ss << " edgeThickness " << m_edgeThickness;
}

void SDFBoxFrame::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFBoxFrame //\n";
    std::string propName{ "" };
    ss << "\tpmVec30 = " << (getShaderAnimationCalls(propName = "dimensions") ? propName : m_dimensions.getShader()) << ";\n";
    ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "edgeThickness") ? propName : std::to_string(m_edgeThickness)) << ";\n";
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

void SDFBoxFrame::bindAnimation(std::unique_ptr<Animation> animation)
{
    if (animation->propName() == "color") {
        animation->BindToProperty(&m_color);
        m_animations.push_back(std::move(animation));
    }
    else if (animation->propName() == "dimensions") {
        animation->BindToProperty(&m_dimensions);
        m_animations.push_back(std::move(animation));
    }
    else if (animation->propName() == "edgeThickness") {
        animation->BindToProperty(&m_edgeThickness);
        m_animations.push_back(std::move(animation));
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
    ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\td" << level << " = sphere(p" << level << ", pmFloat0);\n\n";
}

double SDFSphere::signedDistanceInner(Vec3 p) const
{
    return p.length() - m_radius;
}

void SDFSphere::bindAnimation(std::unique_ptr<Animation> animation)
{
    if (animation->propName() == "color") {
        animation->BindToProperty(&m_color);
        m_animations.push_back(std::move(animation));
    }
    else if (animation->propName() == "radius") {
        animation->BindToProperty(&m_radius);
        m_animations.push_back(std::move(animation));
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
    ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "edgeLength") ? propName : std::to_string(m_edgeLength)) << ";\n";
    ss << "\td" << level << " = crystal(p" << level << ", pmFloat0);\n\n";
}

double SDFCrystal::signedDistanceInner(Vec3 p) const
{
    p = p.abs();
    return (p.x + p.y + p.z - m_edgeLength) * 0.57735027;
}

void SDFCrystal::bindAnimation(std::unique_ptr<Animation> animation)
{
    if (animation->propName() == "color") {
        animation->BindToProperty(&m_color);
        m_animations.push_back(std::move(animation));
    }
    else if (animation->propName() == "edgeLength") {
        animation->BindToProperty(&m_edgeLength);
        m_animations.push_back(std::move(animation));
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
    ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "height") ? propName : std::to_string(m_height)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderAnimationCalls(propName = "sweep") ? propName : std::to_string(m_sweep)) << ";\n";
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

void SDFCone::bindAnimation(std::unique_ptr<Animation> animation)
{
    if (animation->propName() == "color") {
        animation->BindToProperty(&m_color);
        m_animations.push_back(std::move(animation));
    }
    else if (animation->propName() == "height") {
        animation->BindToProperty(&m_height);
        m_animations.push_back(std::move(animation));
    }
    else if (animation->propName() == "sweep") {
        animation->BindToProperty(&m_sweep);
        m_animations.push_back(std::move(animation));
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
    ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "sweep") ? propName : std::to_string(m_sweep)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderAnimationCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
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

void SDFSolidAngle::bindAnimation(std::unique_ptr<Animation> animation)
{
    if (animation->propName() == "color") {
        animation->BindToProperty(&m_color);
        m_animations.push_back(std::move(animation));
    }
    else if (animation->propName() == "radius") {
        animation->BindToProperty(&m_radius);
        m_animations.push_back(std::move(animation));
    }
    else if (animation->propName() == "sweep") {
        animation->BindToProperty(&m_sweep);
        m_animations.push_back(std::move(animation));
    }
}

void SDFEllipsoid::serializeInner(std::stringstream& ss) const
{
    ss << " radii " << m_radii;
}

void SDFEllipsoid::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFEllipsoid //\n";
    std::string propName{ "" };
    ss << "\tpmVec30 = " << (getShaderAnimationCalls(propName = "radii") ? propName : m_radii.getShader()) << ";\n";
    ss << "\td" << level << " = ellipsoid(p" << level << ", pmVec30);\n\n";
}

double SDFEllipsoid::signedDistanceInner(Vec3 p) const
{
    double k0 = (p / m_radii).length();
    double k1 = (p / (m_radii * m_radii)).length();
    return k0 * (k0 - 1.0) / k1;
}

void SDFEllipsoid::bindAnimation(std::unique_ptr<Animation> animation)
{
    if (animation->propName() == "color") {
        animation->BindToProperty(&m_color);
        m_animations.push_back(std::move(animation));
    }
    else if (animation->propName() == "radii") {
        animation->BindToProperty(&m_radii);
        m_animations.push_back(std::move(animation));
    }
}
