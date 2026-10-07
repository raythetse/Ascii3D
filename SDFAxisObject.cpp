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

void SDFTorus::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
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
    ss << "\t// SDFTorusSegmentXY //\n";
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

void SDFTorusSegment::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
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
