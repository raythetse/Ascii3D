#include "SDF2DObject.h"

void SDF2DObject::serializeInner(std::stringstream& ss) const
{
    ss << " magnitude " << m_magnitude;
    ss << " operation " << m_operation;
    ss << " revolveRotations " << m_revolveRotations;
    ss << " revolveRotateOffset " << m_revolveRotateOffset;
    serializeInnerInner(ss);
}

double SDF2DObject::signedDistanceInner(Vec3 p) const
{
    if (m_operation == OP2D_REVOLVE) {
        p = revolve2D(p, m_magnitude);
        p = p.rotateYAxis(m_revolveRotations * MF::TAU);
        p -= Vec3{ m_revolveRotateOffset.x, 0.0, m_revolveRotateOffset.y };
    }
    double d2D = signedDistanceInnerInner(p);
    return m_operation == OP2D_REVOLVE ? d2D : extrude2D(p.y, d2D, m_magnitude);
}

void SDF2DObject::bindPropertyModifier(std::unique_ptr<Modifier> propMod)
{
    if (propMod->propName() == "magnitude") {
        propMod->BindToProp(&m_magnitude);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "revolveRotations") {
        propMod->BindToProp(&m_revolveRotations);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "revolveRotateOffset") {
        propMod->BindToProp(&m_revolveRotateOffset);
        m_propMods.push_back(std::move(propMod));
    }
    else bindPropertyModifierInner(std::move(propMod));
}

void SDFRhombus::serializeInnerInner(std::stringstream& ss) const
{
    ss << " dimensions " << m_dimensions;
}

void SDFRhombus::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFRhombus //\n";
    std::string propName{ "" };
    ss << "\tpmVec20 = " << (getShaderPropModCalls(propName = "dimensions") ? propName : m_dimensions.getShader()) << ";\n";
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "revolveRotations") ? propName : std::to_string(m_revolveRotations)) << ";\n";
    ss << "\tpmVec22 = " << (getShaderPropModCalls(propName = "revolveRotateOffset") ? propName : m_revolveRotateOffset.getShader()) << ";\n";
    ss << "\td" << level << " = rhombus(p" << level << ", pmVec20, " << m_operation << ", pmFloat0, pmFloat1, pmVec22);\n\n";
}

double SDFRhombus::signedDistanceInnerInner(Vec3 p) const
{
    Vec2 dim2D{ m_dimensions.x, -m_dimensions.y };
    Vec2 p2D{ p.x, p.z };
    p2D = p2D.abs();
    double h = std::clamp((dim2D.dot(p2D) + dim2D.y * dim2D.y) / dim2D.dot(dim2D), 0.0, 1.0);
    p2D -= dim2D * Vec2{ h, h - 1.0 };
    return p2D.length() * (p2D.x >= 0.0 ? 1.0 : -1.0);
}

void SDFRhombus::bindPropertyModifierInner(std::unique_ptr<Modifier> propMod)
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

void SDFEquilateralTriangle::serializeInnerInner(std::stringstream& ss) const
{
    ss << " base " << m_base;
}

void SDFEquilateralTriangle::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFEquilateralTriangle //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "base") ? propName : std::to_string(m_base)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
    ss << "\tpmFloat2 = " << (getShaderPropModCalls(propName = "revolveRotations") ? propName : std::to_string(m_revolveRotations)) << ";\n";
    ss << "\tpmVec22 = " << (getShaderPropModCalls(propName = "revolveRotateOffset") ? propName : m_revolveRotateOffset.getShader()) << ";\n";
    ss << "\td" << level << " = equilateralTriangle(p" << level << ", pmFloat0, " << m_operation << ", pmFloat1, pmFloat2, pmVec22 );\n\n";
}

double SDFEquilateralTriangle::signedDistanceInnerInner(Vec3 p) const
{
    double halfBase = m_base * 0.5;
    Vec2 p2D{ p.x, p.z - m_base * MF::ROOT3 / 6.0 + halfBase };
    p2D.x = std::abs(p2D.x);
    p2D -= Vec2{ 0.5, 0.5 * MF::ROOT3 } * std::max(p2D.x + MF::ROOT3 * p2D.y, 0.0);
    p2D -= Vec2{ std::clamp(p2D.x, -halfBase, halfBase), -halfBase / MF::ROOT3 };
    return p2D.length() * (p2D.y < 0.0 ? 1.0 : -1.0);
}

void SDFEquilateralTriangle::bindPropertyModifierInner(std::unique_ptr<Modifier> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "base") {
        propMod->BindToProp(&m_base);
        m_propMods.push_back(std::move(propMod));
    }
}

void SDFIsoscelesTriangle::serializeInnerInner(std::stringstream& ss) const
{
    ss << " dimensions " << m_dimensions;
}

void SDFIsoscelesTriangle::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFIsoscelesTriangle //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
    ss << "\tpmVec20 = " << (getShaderPropModCalls(propName = "dimensions") ? propName : m_dimensions.getShader()) << ";\n";
    ss << "\tpmFloat2 = " << (getShaderPropModCalls(propName = "revolveRotations") ? propName : std::to_string(m_revolveRotations)) << ";\n";
    ss << "\tpmVec22 = " << (getShaderPropModCalls(propName = "revolveRotateOffset") ? propName : m_revolveRotateOffset.getShader()) << ";\n";
    ss << "\td" << level << " = isoscelesTriangle(p" << level << ", pmVec20, " << m_operation << ", pmFloat0, pmFloat2, pmVec22);\n\n";
}

double SDFIsoscelesTriangle::signedDistanceInnerInner(Vec3 p) const
{
    Vec2 p2D{ std::abs(p.x), p.z };
    Vec2 a = p2D - m_dimensions * std::clamp(p2D.dot(m_dimensions) / m_dimensions.dot(m_dimensions), 0.0, 1.0);
    Vec2 b = p2D - m_dimensions * Vec2{ std::clamp(p2D.x / m_dimensions.x, 0.0, 1.0), 1.0 };
    double s = m_dimensions.y >= 0.0 ? -1.0 : 1.0;
    Vec2 d = Vec2{ a.dot(a), s * (p2D.x * m_dimensions.y - p2D.y * m_dimensions.x) }.minimum(Vec2{ b.dot(b), s * (p2D.y - m_dimensions.y) });
    return std::sqrt(d.x) * (d.y >= 0.0 ? -1.0 : 1.0);
}

void SDFIsoscelesTriangle::bindPropertyModifierInner(std::unique_ptr<Modifier> propMod)
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

void SDFTriangle::serializeInnerInner(std::stringstream& ss) const
{
    ss << " point1 " << m_point1;
    ss << " point2 " << m_point2;
    ss << " point3 " << m_point3;
}

void SDFTriangle::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFTriangle //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
    ss << "\tpmVec20 = " << (getShaderPropModCalls(propName = "point1") ? propName : m_point1.getShader()) << ";\n";
    ss << "\tpmVec21 = " << (getShaderPropModCalls(propName = "point2") ? propName : m_point2.getShader()) << ";\n";
    ss << "\tpmVec22 = " << (getShaderPropModCalls(propName = "point3") ? propName : m_point3.getShader()) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "revolveRotations") ? propName : std::to_string(m_revolveRotations)) << ";\n";
    ss << "\tpmVec22 = " << (getShaderPropModCalls(propName = "revolveRotateOffset") ? propName : m_revolveRotateOffset.getShader()) << ";\n";
    ss << "\td" << level << " = triangle(p" << level << ", pmVec20, pmVec21, pmVec22, " << m_operation << ", pmFloat0, pmFloat1, pmVec22);\n\n";
}

double SDFTriangle::signedDistanceInnerInner(Vec3 p) const
{
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
    return -std::sqrt(d.x) * (d.y >= 0.0 ? 1.0 : -1.0);
}

void SDFTriangle::bindPropertyModifierInner(std::unique_ptr<Modifier> propMod)
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
}

void SDFPieSlice::serializeInnerInner(std::stringstream& ss) const
{
    ss << " sweep " << m_sweep;
    ss << " radius " << m_radius;
}

void SDFPieSlice::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFPieSlice //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "sweep") ? propName : std::to_string(m_sweep)) << ";\n";
    ss << "\tpmFloat2 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\tpmVec32.x = " << (getShaderPropModCalls(propName = "revolveRotations") ? propName : std::to_string(m_revolveRotations)) << ";\n";
    ss << "\tpmVec22 = " << (getShaderPropModCalls(propName = "revolveRotateOffset") ? propName : m_revolveRotateOffset.getShader()) << ";\n";
    ss << "\td" << level << " = pieSlice(p" << level << ", pmFloat1, pmFloat2, " << m_operation << ", pmFloat0, pmVec32.x, pmVec22);\n\n";
}

double SDFPieSlice::signedDistanceInnerInner(Vec3 p) const
{
    Vec2 p2D{ p.x, p.z };
    p2D.x = std::abs(p2D.x);
    double l = p2D.length() - m_radius;
    Vec2 sincos = MF::fast_sincos(m_sweep * MF::PI);
    double m = (p2D - sincos * std::clamp(p2D.dot(sincos), 0.0, m_radius)).length(); // c=sin/cos of aperture
    return std::max(l, m * (sincos.y * p2D.x - sincos.x * p2D.y >= 0.0 ? 1.0 : -1.0));
}

void SDFPieSlice::bindPropertyModifierInner(std::unique_ptr<Modifier> propMod)
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
}

void SDFPentagon::serializeInnerInner(std::stringstream& ss) const
{
    ss << " radius " << m_radius;
}

void SDFPentagon::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFPentagon //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\tpmFloat2 = " << (getShaderPropModCalls(propName = "revolveRotations") ? propName : std::to_string(m_revolveRotations)) << ";\n";
    ss << "\tpmVec22 = " << (getShaderPropModCalls(propName = "revolveRotateOffset") ? propName : m_revolveRotateOffset.getShader()) << ";\n";
    ss << "\td" << level << " = pentagon(p" << level << ", pmFloat1, " << m_operation << ", pmFloat0, pmFloat2, pmVec22);\n\n";
}

double SDFPentagon::signedDistanceInnerInner(Vec3 p) const
{
    const Vec3 k{ 0.809016994, 0.587785252, 0.726542528 };
    Vec2 p2D{ std::abs(p.x), p.z };
    p2D -= Vec2{ -k.x, k.y } * std::min(Vec2{ -k.x, k.y }.dot(p2D), 0.0) * 2.0;
    p2D -= Vec2{ k.x, k.y } * std::min(Vec2{ k.x, k.y }.dot(p2D), 0.0) * 2.0;
    p2D -= Vec2{ std::clamp(p2D.x, -m_radius * k.z, m_radius * k.z), m_radius };
    return p2D.length() * (p2D.y >= 0.0 ? 1.0 : -1.0);
}

void SDFPentagon::bindPropertyModifierInner(std::unique_ptr<Modifier> propMod)
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

void SDFHexagon::serializeInnerInner(std::stringstream& ss) const
{
    ss << " radius " << m_radius;
}

void SDFHexagon::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFHexagon //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\tpmFloat2 = " << (getShaderPropModCalls(propName = "revolveRotations") ? propName : std::to_string(m_revolveRotations)) << ";\n";
    ss << "\tpmVec22 = " << (getShaderPropModCalls(propName = "revolveRotateOffset") ? propName : m_revolveRotateOffset.getShader()) << ";\n";
    ss << "\td" << level << " = hexagon(p" << level << ", pmFloat1, " << m_operation << ", pmFloat0, pmFloat2, pmVec22);\n\n";
}

double SDFHexagon::signedDistanceInnerInner(Vec3 p) const
{
    const Vec3 k{ -0.866025404, 0.5, 0.577350269 };
    Vec2 p2D{ std::abs(p.x), std::abs(p.z) };
    p2D -= Vec2{ k.x, k.y } * std::min(Vec2{ k.x, k.y }.dot(p2D), 0.0) * 2.0;
    p2D -= Vec2{ std::clamp(p2D.x, -k.z * m_radius, k.z * m_radius), m_radius };
    return p2D.length() * (p2D.y >= 0.0 ? 1.0 : -1.0);
}

void SDFHexagon::bindPropertyModifierInner(std::unique_ptr<Modifier> propMod)
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

void SDFOctagon::serializeInnerInner(std::stringstream& ss) const
{
    ss << " radius " << m_radius;
}

void SDFOctagon::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFOctagon //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\tpmFloat2 = " << (getShaderPropModCalls(propName = "revolveRotations") ? propName : std::to_string(m_revolveRotations)) << ";\n";
    ss << "\tpmVec22 = " << (getShaderPropModCalls(propName = "revolveRotateOffset") ? propName : m_revolveRotateOffset.getShader()) << ";\n";
    ss << "\td" << level << " = octagon(p" << level << ", pmFloat1, " << m_operation << ", pmFloat0, pmFloat2, pmVec22);\n\n";
}

double SDFOctagon::signedDistanceInnerInner(Vec3 p) const
{
    const Vec3 k{ -0.9238795325, 0.3826834323, 0.4142135623 };
    Vec2 p2D{ std::abs(p.x), std::abs(p.z) };
    p2D -= Vec2{ k.x, k.y } * std::min(Vec2{ k.x, k.y }.dot(p2D), 0.0) * 2.0;
    p2D -= Vec2{ -k.x, k.y } * std::min(Vec2{ -k.x, k.y }.dot(p2D), 0.0) * 2.0;
    p2D -= Vec2{ std::clamp(p2D.x, -k.z * m_radius, k.z * m_radius), m_radius };
    return p2D.length() * (p2D.y >= 0.0 ? 1.0 : -1.0);
}

void SDFOctagon::bindPropertyModifierInner(std::unique_ptr<Modifier> propMod)
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

void SDFStar::serializeInnerInner(std::stringstream& ss) const
{
    ss << " numPoints " << m_numPoints;
    ss << " radius " << m_radius;
    ss << " ratio " << m_ratio;
}

void SDFStar::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFStar //\n";
    std::string propName{ "" };
    ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
    ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "radius") ? propName : std::to_string(m_radius)) << ";\n";
    ss << "\tpmFloat2 = " << (getShaderPropModCalls(propName = "ratio") ? propName : std::to_string(m_ratio)) << ";\n";
    ss << "\tpmVec20.x = " << (getShaderPropModCalls(propName = "numPoints") ? propName : std::to_string(m_numPointsD)) << ";\n";
    ss << "\tpmVec20.y = " << (getShaderPropModCalls(propName = "revolveRotations") ? propName : std::to_string(m_revolveRotations)) << ";\n";
    ss << "\tpmVec22 = " << (getShaderPropModCalls(propName = "revolveRotateOffset") ? propName : m_revolveRotateOffset.getShader()) << ";\n";
    ss << "\td" << level << " = star(p" << level << ", pmVec20.x, pmFloat1, pmFloat2, " << m_operation << ", pmFloat0, pmVec20.y, pmVec22);\n\n";
}

double SDFStar::signedDistanceInnerInner(Vec3 p) const
{
    Vec2 asincos = MF::fast_sincos(MF::PI / m_numPointsD);
    Vec2 acs{ asincos.y, asincos.x };
    Vec2 esincos{ MF::fast_sincos(MF::PI / std::lerp(2.0, m_numPointsD, m_ratio)) };
    Vec2 ecs{ esincos.y, esincos.x };
    double an{ MF::PI / m_numPointsD };

    Vec2 p2D{ abs(p.x), p.z };
    double bn = std::fmod(std::atan2(p2D.x, p2D.y), 2.0 * an) - an;
    Vec2 bnsincos = MF::fast_sincos(bn);
    p2D = Vec2{ bnsincos.y, std::abs(bnsincos.x) } * p2D.length();
    p2D -= acs * m_radius;
    p2D += ecs * std::clamp(-(p2D.dot(ecs)), 0.0, m_radius * acs.y / ecs.y);
    return p2D.length() * (p2D.x >= 0.0 ? 1.0 : -1.0);
}

void SDFStar::bindPropertyModifierInner(std::unique_ptr<Modifier> propMod)
{
    if (propMod->propName() == "color") {
        propMod->BindToProp(&m_color);
        m_propMods.push_back(std::move(propMod));
    }
    else if (propMod->propName() == "numPoints") {
        propMod->BindToProp(&m_numPointsD);
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
}
