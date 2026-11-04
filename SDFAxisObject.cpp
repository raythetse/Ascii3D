#include "SDFAxisObject.h"

Vec3 SDFAxisObject::getP(Vec3 p) const
{
    switch (m_axis)
    {
    case AXIS_X:
        return Vec3(p.y, p.x, p.z);
    case AXIS_Y:
        return p;
    case AXIS_Z:
        return Vec3(p.x, p.z, p.y);
    }
    return p;
}

std::string SDFAxisObject::getPSwizzle() const
{
    switch (m_axis)
    {
    case AXIS_X:
        return ".yxz";
    case AXIS_Y:
        return ".xyz";
    case AXIS_Z:
        return ".xzy";
    }
    return ".xyz";
}

void SDFTorus::serializeInner(std::stringstream& ss) const
{
    ss << " radii " << m_radii;
    ss << " axis " << m_axis;
}

void SDFTorus::getShaderObjectCalls(std::stringstream& ss, size_t level) const
{
    ss << "\t// SDFTorus //\n";
    std::string propName{ "" };
    ss << "\tpmVec20 = " << (getShaderAnimationCalls(propName = "radii") ? propName : m_radii.getShader()) << ";\n";
    ss << "\td" << level << " = torus(p" << level << getPSwizzle() << ", pmVec20);\n\n";
}

double SDFTorus::signedDistanceInner(Vec3 p) const
{
    p = getP(p);
    return Vec2{ Vec2{p.x, p.z}.length() - m_radii.x, p.y }.length() - m_radii.y;
}

BindResult SDFTorus::bindAnimation(std::unique_ptr<Animation> animation)
{
    if (animation->propName() == "color") return attach(m_animations, std::move(animation), m_color);
    if (animation->propName() == "radii") return attach(m_animations, std::move(animation), m_radii);
    return BindResult::UnknownProperty;
}

void SDFTorusSegment::serializeInner(std::stringstream& ss) const
{
    ss << " radii " << m_radii;
    ss << " sweep " << m_sweep;
    ss << " axis " << m_axis;
}

void SDFTorusSegment::getShaderObjectCalls(std::stringstream& ss, size_t level) const
{
    ss << "\t// SDFTorusSegment //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "sweep") ? propName : std::to_string(m_sweep)) << ";\n";
    ss << "\tpmVec20 = " << (getShaderAnimationCalls(propName = "radii") ? propName : m_radii.getShader()) << ";\n";
    ss << "\td" << level << " = torusSegment(p" << level << getPSwizzle() << ", pmVec20, pmFloat0);\n\n";
}

double SDFTorusSegment::signedDistanceInner(Vec3 p) const
{
    p = getP(p);
    p.x = abs(p.x);
    Vec2 pXZ{ p.x, p.z };
    Vec2 sincos = MF::fast_sincos(m_sweep * MF::PI);
    double k = (sincos.y * p.x > sincos.x * p.z) ? pXZ.dot(sincos) : pXZ.length();
    return sqrt(p.dot(p) + m_radii.x * m_radii.x - 2.0 * m_radii.x * k) - m_radii.y;
}

BindResult SDFTorusSegment::bindAnimation(std::unique_ptr<Animation> animation)
{
    if (animation->propName() == "color") return attach(m_animations, std::move(animation), m_color);
    if (animation->propName() == "radii") return attach(m_animations, std::move(animation), m_radii);
    if (animation->propName() == "sweep") return attach(m_animations, std::move(animation), m_sweep);
    return BindResult::UnknownProperty;
}

void SDFCapsule::serializeInner(std::stringstream& ss) const
{
    ss << " radius " << m_radius;
    ss << " bodyLength " << m_bodyLength;
    ss << " axis " << m_axis;
}

void SDFCapsule::getShaderObjectCalls(std::stringstream& ss, size_t level) const
{
    ss << "\t// SDFCapsule //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "bodyLength") ? propName : std::to_string(m_bodyLength)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderAnimationCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\td" << level << " = capsule(p" << level << getPSwizzle() << ", pmFloat0, pmFloat1);\n\n";
}

double SDFCapsule::signedDistanceInner(Vec3 p) const
{
    p = getP(p);
    double halfBL = m_bodyLength * 0.5;
    p.y -= std::clamp(p.y, -halfBL, halfBL);
    return p.length() - m_radius;
}

BindResult SDFCapsule::bindAnimation(std::unique_ptr<Animation> animation)
{
    if (animation->propName() == "color") return attach(m_animations, std::move(animation), m_color);
    if (animation->propName() == "radius") return attach(m_animations, std::move(animation), m_radius);
    if (animation->propName() == "bodyLength") return attach(m_animations, std::move(animation), m_bodyLength);
    return BindResult::UnknownProperty;
}

void SDFTaperedCapsule::serializeInner(std::stringstream& ss) const
{
    ss << " bodyLength " << m_bodyLength;
    ss << " radii " << m_radii;
    ss << " axis " << m_axis;
}

void SDFTaperedCapsule::getShaderObjectCalls(std::stringstream& ss, size_t level) const
{
    ss << "\t// SDFTaperedCapsule //\n";
    std::string propName{ "" };
    ss << "\tpmVec20 = " << (getShaderAnimationCalls(propName = "radii") ? propName : m_radii.getShader()) << ";\n";
    ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "bodyLength") ? propName : std::to_string(m_bodyLength)) << ";\n";
    ss << "d" << level << " = taperedCapsule(p" << level << getPSwizzle() << ", pmVec20, pmFloat0); \n\n";
}

double SDFTaperedCapsule::signedDistanceInner(Vec3 p) const
{
    double b = (m_radii.x - m_radii.y) / m_bodyLength;
    double a = sqrt(1.0 - b * b);
    p = getP(p);
    Vec2 q{ Vec2{p.x, p.z}.length(), p.y };
    double k = q.dot(Vec2{ -b, a });
    if (k < 0.0) return q.length() - m_radii.x;
    if (k > a * m_bodyLength) return (q - Vec2{ 0.0, m_bodyLength }).length() - m_radii.y;
    return q.dot(Vec2{ a, b }) - m_radii.x;
}

BindResult SDFTaperedCapsule::bindAnimation(std::unique_ptr<Animation> animation)
{
    if (animation->propName() == "color") return attach(m_animations, std::move(animation), m_color);
    if (animation->propName() == "bodyLength") return attach(m_animations, std::move(animation), m_bodyLength);
    if (animation->propName() == "radii") return attach(m_animations, std::move(animation), m_radii);
    return BindResult::UnknownProperty;
}

void SDFCylinder::serializeInner(std::stringstream& ss) const
{
    ss << " sideLength " << m_sideLength;
    ss << " radius " << m_radius;
    ss << " axis " << m_axis;
}

void SDFCylinder::getShaderObjectCalls(std::stringstream& ss, size_t level) const
{
    ss << "\t// SDFCylinder //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "sideLength") ? propName : std::to_string(m_sideLength)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderAnimationCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\td" << level << " = cylinder(p" << level << getPSwizzle() << ", pmFloat0, pmFloat1);\n\n";
}

double SDFCylinder::signedDistanceInner(Vec3 p) const
{
    p = getP(p);
    Vec2 xz{ p.x, p.z };
    Vec2 d = Vec2{ xz.length(), p.y }.abs() - Vec2(m_radius, m_sideLength * 0.5);
    return std::min(std::max(d.x, d.y), 0.0) + d.maximum(0.0).length();
}

BindResult SDFCylinder::bindAnimation(std::unique_ptr<Animation> animation)
{
    if (animation->propName() == "color") return attach(m_animations, std::move(animation), m_color);
    if (animation->propName() == "radius") return attach(m_animations, std::move(animation), m_radius);
    if (animation->propName() == "sideLength") return attach(m_animations, std::move(animation), m_sideLength);
    return BindResult::UnknownProperty;
}

void SDFTaperedCylinder::serializeInner(std::stringstream& ss) const
{
    ss << " sideLength " << m_sideLength;
    ss << " radii " << m_radii;
    ss << " axis " << m_axis;
}

void SDFTaperedCylinder::getShaderObjectCalls(std::stringstream& ss, size_t level) const
{
    ss << "\t// SDFTaperedCylinder //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "sideLength") ? propName : std::to_string(m_sideLength)) << ";\n";
    ss << "\tpmVec20 = " << (getShaderAnimationCalls(propName = "radii") ? propName : m_radii.getShader()) << ";\n";
    ss << "\td" << level << " = taperedCylinder(p" << level << getPSwizzle() << ", pmFloat0, pmVec20);\n\n";
}

double SDFTaperedCylinder::signedDistanceInner(Vec3 p) const
{
    p = getP(p);
    double halfLength = 0.5 * m_sideLength;
    Vec2 q = Vec2(Vec2(p.x, p.z).length(), p.y);
    Vec2 k1 = Vec2(m_radii.y, halfLength);
    Vec2 k2 = Vec2(m_radii.y - m_radii.x, 2.0 * halfLength);
    Vec2 ca = Vec2(q.x - std::min(q.x, (q.y < 0.0) ? m_radii.x : m_radii.y), abs(q.y) - halfLength);
    Vec2 cb = q - k1 + k2 * std::clamp((k1 - q).dot(k2) / k2.dot(k2), 0.0, 1.0);
    double s = (cb.x < 0.0 && ca.y < 0.0) ? -1.0 : 1.0;
    return s * std::sqrt(std::min(ca.dot(ca), cb.dot(cb)));
}

BindResult SDFTaperedCylinder::bindAnimation(std::unique_ptr<Animation> animation)
{
    if (animation->propName() == "color") return attach(m_animations, std::move(animation), m_color);
    if (animation->propName() == "radii") return attach(m_animations, std::move(animation), m_radii);
    if (animation->propName() == "sideLength") return attach(m_animations, std::move(animation), m_sideLength);
    return BindResult::UnknownProperty;
}
