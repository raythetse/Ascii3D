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
    ss << " radii " << m_radii.x << ' ' << m_radii.y;
    ss << " axis " << m_axis;
}

void SDFTorus::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFTorus //\n";
    std::string propName{ "" };
    ss << "\tpmVec20 = " << (getShaderPropModCalls(propName = "radii") ? propName : m_radii.getShader()) << ";\n";
    ss << "\td" << level << " = torus(p" << level << getPSwizzle() << ", pmVec20);\n\n";
}

double SDFTorus::signedDistanceInner(Vec3 p) const
{
    p = getP(p);
    return Vec2{ Vec2{p.x, p.z}.length() - m_radii.x, p.y }.length() - m_radii.y;
}

void SDFTorus::bindPropertyModifier(std::unique_ptr<Modifier> propMod)
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

void SDFTorusSegment::serializeInner(std::stringstream& ss) const
{
    ss << " radii " << m_radii.x << ' ' << m_radii.y;
    ss << " sweep " << m_sweep;
    ss << " axis " << m_axis;
}

void SDFTorusSegment::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFTorusSegment //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "sweep") ? propName : std::to_string(m_sweep)) << ";\n";
    ss << "\tpmVec20 = " << (getShaderPropModCalls(propName = "radii") ? propName : m_radii.getShader()) << ";\n";
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

void SDFTorusSegment::bindPropertyModifier(std::unique_ptr<Modifier> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "radii") {
        propMod->BindToProp(&m_radii);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "sweep") {
        propMod->BindToProp(&m_sweep);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFCapsule::serializeInner(std::stringstream& ss) const
{
    ss << " radius " << m_radius;
    ss << " bodyLength " << m_bodyLength;
    ss << " axis " << m_axis;
}

void SDFCapsule::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFCapsule //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "bodyLength") ? propName : std::to_string(m_bodyLength)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\td" << level << " = capsule(p" << level << getPSwizzle() << ", pmFloat0, pmFloat1);\n\n";
}

double SDFCapsule::signedDistanceInner(Vec3 p) const
{
    p = getP(p);
    double halfBL = m_bodyLength * 0.5;
    p.y -= std::clamp(p.y, -halfBL, halfBL);
    return p.length() - m_radius;
}

void SDFCapsule::bindPropertyModifier(std::unique_ptr<Modifier> propMod)
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
    ss << " axis " << m_axis;
}

void SDFTaperedCapsule::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFTaperedCapsule //\n";
    std::string propName{ "" };
    ss << "\tpmVec20 = " << (getShaderPropModCalls(propName = "radii") ? propName : m_radii.getShader()) << ";\n";
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "bodyLength") ? propName : std::to_string(m_bodyLength)) << ";\n";
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

void SDFTaperedCapsule::bindPropertyModifier(std::unique_ptr<Modifier> propMod)
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

void SDFCylinder::serializeInner(std::stringstream& ss) const
{
    ss << " sideLength " << m_sideLength;
    ss << " radius " << m_radius;
    ss << " axis " << m_axis;
}

void SDFCylinder::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFCylinder //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "sideLength") ? propName : std::to_string(m_sideLength)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\td" << level << " = cylinder(p" << level << getPSwizzle() << ", pmFloat0, pmFloat1);\n\n";
}

double SDFCylinder::signedDistanceInner(Vec3 p) const
{
    p = getP(p);
    Vec2 xz{ p.x, p.z };
    Vec2 d = Vec2{ xz.length(), p.y }.abs() - Vec2(m_radius, m_sideLength * 0.5);
    return std::min(std::max(d.x, d.y), 0.0) + d.maximum(0.0).length();
}

void SDFCylinder::bindPropertyModifier(std::unique_ptr<Modifier> propMod)
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
