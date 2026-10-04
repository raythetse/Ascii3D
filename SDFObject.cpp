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
    ss << "\tcolor" << level << " = " << m_color.getShader() << ";\n";
    getShaderObjectCalls(ss, level, maxLevel);
    ss << "\n";

    for (auto& op : m_operations) {
        op->getShaderCalls(ss, level, maxLevel);
    }
}

double SDFObject::signedDistanceInner(Vec3 p) const
{
    return std::numeric_limits<double>::max();
}

void SDFPlaneXZ::serializeInner(std::stringstream& ss) const
{
    ss << " yPos " << m_yPos;
}

void SDFPlaneXZ::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFPlaneXZ //\n";
    ss << "\td" << level << " = p" << level << ".y - " << m_yPos << ";\n\n";
}

double SDFPlaneXZ::signedDistanceInner(Vec3 p) const
{
    return p.y - m_yPos;
}

void SDFPlaneYZ::serializeInner(std::stringstream& ss) const
{
    ss << " xPos " << m_xPos;
}

void SDFPlaneYZ::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFPlaneYZ //\n";
    ss << "\td" << level << " = p" << level << ".x - " << m_xPos << ";\n\n";
}

double SDFPlaneYZ::signedDistanceInner(Vec3 p) const
{
    return p.x - m_xPos;
}

void SDFPlaneXY::serializeInner(std::stringstream& ss) const
{
    ss << " zPos " << m_zPos;
}

void SDFPlaneXY::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFPlaneXY //\n";
    ss << "\td" << level << " = p" << level << ".z - " << m_zPos << ";\n\n";
}

double SDFPlaneXY::signedDistanceInner(Vec3 p) const
{
    return p.z - m_zPos;
}

void SDFPlane::serializeInner(std::stringstream& ss) const
{
    ss << " normal " << m_normal.x << ' ' << m_normal.y << ' ' << m_normal.z;
    ss << " offset " << m_offset;
}

void SDFPlane::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFPlaneXY //\n";
    ss << "\td" << level << " = dot(p" << level << ", " << m_normal.getShader() << ") + " << m_offset << ";\n\n";
}

double SDFPlane::signedDistanceInner(Vec3 p) const
{
    return p.dot(m_normal) + m_offset;
}

void SDFBox::serializeInner(std::stringstream& ss) const
{
    ss << " dimensions " << m_dimensions.x << ' ' << m_dimensions.y << ' ' << m_dimensions.z;
}

void SDFBox::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFBox //\n";
    ss << "\td" << level << " = box(p" << level << ", " << m_dimensions.getShader() << ");\n\n";
}

double SDFBox::signedDistanceInner(Vec3 p) const
{
    p = p.abs();
    p -= Vec3(m_dimensions) * 0.5;
    return p.maximum(0.0).length() + std::min(std::max(p.x, std::max(p.y, p.z)), 0.0);
}

void SDFBoxFrame::serializeInner(std::stringstream& ss) const
{
    ss << " dimensions " << m_dimensions.x << ' ' << m_dimensions.y << ' ' << m_dimensions.z;
    ss << " edgeThickness " << m_edgeThickness;
}

void SDFBoxFrame::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFBoxFrame //\n";
    ss << "\td" << level << " = boxFrame(p" << level << ", " << m_dimensions.getShader() << ", " << m_edgeThickness << ");\n\n";
}

double SDFBoxFrame::signedDistanceInner(Vec3 p) const
{
    p = p.abs() - m_dimensions;
    Vec3 q = (p + m_edgeThickness).abs() - m_edgeThickness;
    return std::min(std::min(
        Vec3(p.x, q.y, q.z).maximum(0.0).length() + std::min(std::max(p.x, std::max(q.y, q.z)), 0.0),
        Vec3(q.x, p.y, q.z).maximum(0.0).length() + std::min(std::max(q.x, std::max(p.y, q.z)), 0.0)),
        Vec3(q.x, q.y, p.z).maximum(0.0).length() + std::min(std::max(q.x, std::max(q.y, p.z)), 0.0));
}

void SDFSphere::serializeInner(std::stringstream& ss) const
{
    ss << " radius " << m_radius;
}

void SDFSphere::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFSphere //\n";
    ss << "\td" << level << " = sphere(p" << level << ", " << m_radius << ");\n\n";
}

double SDFSphere::signedDistanceInner(Vec3 p) const
{
    return p.length() - m_radius;
}

void SDFTorusXY::serializeInner(std::stringstream& ss) const
{
    ss << " radii " << m_radii.x << ' ' << m_radii.y;
}

void SDFTorusXY::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFTorusXY //\n";
    ss << "\td" << level << " = torus(p" << level << ".xzy, " << m_radii.getShader() << ");\n\n";
}

double SDFTorusXY::signedDistanceInner(Vec3 p) const
{
    return Vec2{ Vec2{p.x, p.y}.length() - m_radii.x, p.z }.length() - m_radii.y;
}

void SDFTorusXZ::serializeInner(std::stringstream& ss) const
{
    ss << " radii " << m_radii.x << ' ' << m_radii.y;
}

void SDFTorusXZ::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFTorusXZ //\n";
    ss << "\td" << level << " = torus(p" << level << ", " << m_radii.getShader() << ");\n\n";
}

double SDFTorusXZ::signedDistanceInner(Vec3 p) const
{
    return Vec2{ Vec2{p.x, p.z}.length() - m_radii.x, p.y }.length() - m_radii.y;
}

void SDFTorusYZ::serializeInner(std::stringstream& ss) const
{
    ss << " radii " << m_radii.x << ' ' << m_radii.y;
}

void SDFTorusYZ::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFTorusYZ //\n";
    ss << "\td" << level << " = torus(p" << level << ".yxz, " << m_radii.getShader() << ");\n\n";
}

double SDFTorusYZ::signedDistanceInner(Vec3 p) const
{
    return Vec2{ Vec2{p.y, p.z}.length() - m_radii.x, p.x }.length() - m_radii.y;
}

void SDFTorusSegment::serializeInner(std::stringstream& ss) const
{
    ss << " radii " << m_radii.x << ' ' << m_radii.y;
    ss << " sweep " << m_sweep;
}

void SDFTorusSegment::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFTorus //\n";
    ss << "\td" << level << " = torusSegment(p" << level << ", " << m_radii.getShader() << ", " << m_sincos.getShader() << ");\n\n";
}

double SDFTorusSegment::signedDistanceInner(Vec3 p) const
{
    p.x = abs(p.x);
    Vec2 pXY{ p.x, p.y };
    double k = (m_sincos.y * p.x > m_sincos.x * p.y) ? pXY.dot(m_sincos) : pXY.length();
    return sqrt(p.dot(p) + m_radii.x * m_radii.x - 2.0 * m_radii.x * k) - m_radii.y;
}

void SDFCapsuleX::serializeInner(std::stringstream& ss) const
{
    ss << " radius " << m_radius;
    ss << " bodyLength " << m_bodyLength;
}

void SDFCapsuleX::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFCapsuleX //\n";
    ss << "\td" << level << " = capsuleX(p" << level << ", " << m_bodyLength << ", " << m_radius << ");\n\n";
}

double SDFCapsuleX::signedDistanceInner(Vec3 p) const
{
    p.x = p.x > 0.0 ? std::max(0.0, p.x - 0.5 * m_bodyLength) : std::min(0.0, p.x + 0.5 * m_bodyLength);
    return p.length() - m_radius;
}

void SDFCapsuleY::serializeInner(std::stringstream& ss) const
{
    ss << " radius " << m_radius;
    ss << " bodyLength " << m_bodyLength;
}

void SDFCapsuleY::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFCapsuleY //\n";
    ss << "\td" << level << " = capsuleY(p" << level << ", " << m_bodyLength << ", " << m_radius << ");\n\n";
}

double SDFCapsuleY::signedDistanceInner(Vec3 p) const
{
    p.y = p.y > 0.0 ? std::max(0.0, p.y - 0.5 * m_bodyLength) : std::min(0.0, p.y + 0.5 * m_bodyLength);
    return p.length() - m_radius;
}

void SDFCapsuleZ::serializeInner(std::stringstream& ss) const
{
    ss << " radius " << m_radius;
    ss << " bodyLength " << m_bodyLength;
}

void SDFCapsuleZ::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFCapsuleZ //\n";
    ss << "\td" << level << " = capsuleZ(p" << level << ", " << m_bodyLength << ", " << m_radius << ");\n\n";
}

double SDFCapsuleZ::signedDistanceInner(Vec3 p) const
{
    p.z = p.z > 0.0 ? std::max(0.0, p.z - 0.5 * m_bodyLength) : std::min(0.0, p.z + 0.5 * m_bodyLength);
    return p.length() - m_radius;
}

void SDFCrystal::serializeInner(std::stringstream& ss) const
{
    ss << " edgeLength " << m_edgeLength;
}

void SDFCrystal::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFCrystal //\n";
    ss << "\td" << level << " = crystal(p" << level << ", " << m_edgeLength << ");\n\n";
}

double SDFCrystal::signedDistanceInner(Vec3 p) const
{
    p = p.abs();
    return (p.x + p.y + p.z - m_edgeLength) * 0.57735027;
}

void SDFCylinderX::serializeInner(std::stringstream& ss) const
{
    ss << " sideLength " << m_sideLength;
    ss << " radius " << m_radius;
}

void SDFCylinderX::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFCylinderX //\n";
    ss << "\td" << level << " = cylinderX(p" << level << ", " << m_sideLength << ", " << m_radius << ");\n\n";
}

double SDFCylinderX::signedDistanceInner(Vec3 p) const
{
    Vec2 yz{ p.y, p.z };
    Vec2 d = Vec2{ yz.length(), p.x }.abs() - Vec2(m_radius, m_sideLength * 0.5);
    return std::min(std::max(d.x, d.y), 0.0) + d.maximum(0.0).length();
}

void SDFCylinderY::serializeInner(std::stringstream& ss) const
{
    ss << " sideLength " << m_sideLength;
    ss << " radius " << m_radius;
}

void SDFCylinderY::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFCylinderY //\n";
    ss << "\td" << level << " = cylinderY(p" << level << ", " << m_sideLength << ", " << m_radius << ");\n\n";
}

double SDFCylinderY::signedDistanceInner(Vec3 p) const
{
    Vec2 xz{ p.x, p.z };
    Vec2 d = Vec2{ xz.length(), p.y }.abs() - Vec2(m_radius, m_sideLength * 0.5);
    return std::min(std::max(d.x, d.y), 0.0) + d.maximum(0.0).length();
}

void SDFCylinderZ::serializeInner(std::stringstream& ss) const
{
    ss << " sideLength " << m_sideLength;
    ss << " radius " << m_radius;
}

void SDFCylinderZ::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFCylinderZ //\n";
    ss << "\td" << level << " = cylinderZ(p" << level << ", " << m_sideLength << ", " << m_radius << ");\n\n";
}

double SDFCylinderZ::signedDistanceInner(Vec3 p) const
{
    Vec2 xy{ p.x, p.y };
    Vec2 d = Vec2{ xy.length(), p.z }.abs() - Vec2(m_radius, m_sideLength * 0.5);
    return std::min(std::max(d.x, d.y), 0.0) + d.maximum(0.0).length();
}

void SDFChainLink::serializeInner(std::stringstream& ss) const
{
    ss << " sideLength " << m_sideLength;
    ss << " radii " << m_radii.x << ' ' << m_radii.y;
}

void SDFChainLink::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFChainLink //\n";
    ss << "\td" << level << " = chainLink(p" << level << ", " << m_sideLength << ", " << m_radii.getShader() << ");\n\n";
}

double SDFChainLink::signedDistanceInner(Vec3 p) const
{
    p.x = p.x > 0.0 ? std::max(0.0, p.x - m_sideLength * 0.5) : std::min(0.0, p.x + m_sideLength * 0.5);
    return Vec2{ Vec2{p.x, p.z}.length() - m_radii.x, p.y }.length() - m_radii.y;
}

void SDFTube::serializeInner(std::stringstream& ss) const
{
    ss << " height " << m_height;
    ss << " radii " << m_radii.x << ' ' << m_radii.y;
}

void SDFTube::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFTube //\n";
    ss << "\td" << level << " = tube(p" << level << ", " << m_height << ", " << m_radii.getShader() << ");\n\n";
}

double SDFTube::signedDistanceInner(Vec3 p) const
{
    p.y = p.y > 0.0 ? std::max(0.0, p.y - m_height * 0.5) : std::min(0.0, p.y + m_height * 0.5);
    return Vec2{ Vec2{p.x, p.z}.length() - m_radii.x, p.y }.length() - m_radii.y;
}

void SDFCone::serializeInner(std::stringstream& ss) const
{
    ss << " height " << m_height;
    ss << " ratio " << m_ratio;
}

void SDFCone::getShaderObjectCalls(std::stringstream & ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFCone //\n";
    ss << "\td" << level << " = cone(p" << level << ", " << m_height << ", " << (m_ratio * MF::TAU) << ");\n\n";
}

double SDFCone::signedDistanceInner(Vec3 p) const
{
    Vec2 q = Vec2{ m_sincos.x / m_sincos.y, -1.0 } * m_height;
    Vec2 w = Vec2{ Vec2{ p.x, p.z }.length(), p.y };
    Vec2 a = w - q * std::clamp(w.dot(q) / q.dot(q), 0.0, 1.0);
    Vec2 b = w - q * Vec2{ std::clamp(w.x / q.x, 0.0, 1.0), 1.0 };
    double k = q.y >= 0.0 ? 1.0 : -1.0;
    double d = std::min(a.dot(a), b.dot(b));
    double s = std::max(k * (w.x * q.y - w.y * q.x), k * (w.y - q.y));
    return std::sqrt(d) * (s >= 0.0 ? 1.0 : -1.0);
}

void SDFSolidAngle::serializeInner(std::stringstream& ss) const
{
    ss << " radius " << m_radius;
    ss << " sweep " << m_sweep;
}

void SDFSolidAngle::getShaderObjectCalls(std::stringstream & ss, size_t level, size_t highestLevel) const
{
    ss << "\t// SDFSolidAngle //\n";
    ss << "\td" << level << " = solidAngle(p" << level << ", " << m_sincos.getShader() <<", " <<m_radius << ");\n\n";
}

double SDFSolidAngle::signedDistanceInner(Vec3 p) const
{
    Vec2 q = Vec2{ Vec2{ p.x, p.z }.length(), p.y };
    double l = q.length() - m_radius;
    double m = (q - m_sincos * std::clamp(q.dot(m_sincos), 0.0, m_radius)).length();
    return std::max(l, m * ((m_sincos.y * q.x - m_sincos.x * q.y) > 0.0 ? 1.0 : -1.0));
}

void SDFEllipsoid::serializeInner(std::stringstream& ss) const
{
    ss << " radii " << m_radii.x << ' ' << m_radii.y << ' ' << m_radii.z;
}

void SDFEllipsoid::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "d" << level << " = ellipsoid(p" << level << ", " << m_radii.getShader() << ");\n\n";
}

double SDFEllipsoid::signedDistanceInner(Vec3 p) const
{
    double k0 = (p / m_radii).length();
    double k1 = (p / (m_radii * m_radii)).length();
    return k0 * (k0 - 1.0) / k1;
}

void SDFTaperedCapsule::serializeInner(std::stringstream& ss) const
{
    ss << " height " << m_height;
    ss << " radii " << m_radii.x << ' ' << m_radii.y;
}

void SDFTaperedCapsule::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "d" << level << " = taperedCapsule(p" << level << ", " << m_radii.getShader() << ", " << m_height << "); \n\n";
}

double SDFTaperedCapsule::signedDistanceInner(Vec3 p) const
{
    double b = (m_radii.x - m_radii.y) / m_height;
    double a = sqrt(1.0 - b * b);

    Vec2 q{ Vec2{ p.x, p.z }.length(), p.y };
    double k = q.dot(Vec2{ -b, a });
    if (k < 0.0) return q.length() - m_radii.x;
    if (k > a * m_height) return (q - Vec2{ 0.0, m_height }).length() - m_radii.y;
    return q.dot(Vec2{ a, b }) - m_radii.x;
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
    ss << "\td" << level << " = rhombus(p" << level << ", " << m_dimensions.getShader() << ", " << m_operation << ", " << m_magnitude << ");\n\n";
}

double SDFRhombus::signedDistanceInner(Vec3 p) const
{
    if (m_operation == Revolve) p = revolve2D(p, m_magnitude);
    Vec2 dim2D{ m_dimensions.x, -m_dimensions.y };
    Vec2 p2D{ p.x, p.z };
    p2D = p2D.abs();
    double h = std::clamp((dim2D.dot(p2D) + dim2D.y * dim2D.y) / dim2D.dot(dim2D), 0.0, 1.0);
    p2D -= dim2D * Vec2{ h, h - 1.0 };
    double d2D = p2D.length() * (p2D.x >= 0.0 ? 1.0 : -1.0);
    return m_operation == Revolve ? d2D : extrude2D(p.y, d2D, m_magnitude);
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
    ss << "\td" << level << " = equilateralTriangle(p" << level << ", " << m_base << ", " << m_operation << ", " << m_magnitude << ");\n\n";
}

double SDFEquilateralTriangle::signedDistanceInner(Vec3 p) const
{
    if (m_operation == Revolve) p = revolve2D(p, m_magnitude);
    double halfBase = m_base * 0.5;
    Vec2 p2D{ p.x, p.z - m_base * MF::ROOT3 / 6.0 + halfBase };
    p2D.x = std::abs(p2D.x);
    p2D -= Vec2{ 0.5, 0.5 * MF::ROOT3 } * std::max(p2D.x + MF::ROOT3 * p2D.y, 0.0);
    p2D -= Vec2{ std::clamp(p2D.x, -halfBase, halfBase), -halfBase / MF::ROOT3 };
    double d2D = p2D.length() * (p2D.y < 0.0 ? 1.0 : -1.0);
    return m_operation == Extrude ? extrude2D(p.y, d2D, m_magnitude * 0.5) : d2D;
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
    ss << "\td" << level << " = isoscelesTriangle(p" << level << ", " << m_dimensions.getShader() << ", " << m_operation << ", " << m_magnitude << ");\n\n";
}

double SDFIsoscelesTriangle::signedDistanceInner(Vec3 p) const
{
    if (m_operation == Revolve) p = revolve2D(p, m_magnitude);
    Vec2 p2D{ std::abs(p.x), p.z };
    Vec2 a = p2D - m_dimensions * std::clamp(p2D.dot(m_dimensions) / m_dimensions.dot(m_dimensions), 0.0, 1.0);
    Vec2 b = p2D - m_dimensions * Vec2{ std::clamp(p2D.x / m_dimensions.x, 0.0, 1.0), 1.0 };
    double s = m_dimensions.y >= 0.0 ? -1.0 : 1.0;
    Vec2 d = Vec2{ a.dot(a), s * (p2D.x * m_dimensions.y - p2D.y * m_dimensions.x) }.minimum(Vec2{ b.dot(b), s * (p2D.y - m_dimensions.y) });
    double d2D = std::sqrt(d.x) * (d.y >= 0.0 ? -1.0 : 1.0);
    return m_operation == Extrude ? extrude2D(p.y, d2D, m_magnitude * 0.5) : d2D;
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
    ss << "\td" << level << " = pieSlice(p" << level << ", " << m_sincos.getShader() << ", " << m_radius << ", " << m_operation << ", " << m_magnitude << ");\n\n";
}

double SDFPieSlice::signedDistanceInner(Vec3 p) const
{
    if (m_operation == Revolve) p = revolve2D(p, m_magnitude);
    Vec2 p2D{ p.x, p.z };
    p2D.x = std::abs(p2D.x);
    double l = p2D.length() - m_radius;
    double m = (p2D - m_sincos * std::clamp(p2D.dot(m_sincos), 0.0, m_radius)).length(); // c=sin/cos of aperture
    double d2D = std::max(l, m * (m_sincos.y * p2D.x - m_sincos.x * p2D.y >= 0.0 ? 1.0 : -1.0));
    return m_operation == Extrude ? extrude2D(p.y, d2D, m_magnitude * 0.5) : d2D;
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
    ss << "\td" << level << " = pentagon(p" << level << ", " << m_radius << ", " << m_operation << ", " << m_magnitude << ");\n\n";
}

double SDFPentagon::signedDistanceInner(Vec3 p) const
{
    if (m_operation == Revolve) p = revolve2D(p, m_magnitude);
    const Vec3 k{ 0.809016994, 0.587785252, 0.726542528 };
    Vec2 p2D{ std::abs(p.x), p.z };
    p2D -= Vec2{ -k.x, k.y } * std::min(Vec2{ -k.x, k.y }.dot(p2D), 0.0) * 2.0;
    p2D -= Vec2 { k.x, k.y } * std::min(Vec2{ k.x, k.y }.dot(p2D), 0.0) * 2.0;
    p2D -= Vec2{ std::clamp(p2D.x, -m_radius * k.z, m_radius * k.z), m_radius };
    double d2D = p2D.length() * (p2D.y >= 0.0 ? 1.0 : -1.0);
    return m_operation == Extrude ? extrude2D(p.y, d2D, m_magnitude * 0.5) : d2D;
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
    ss << "\td" << level << " = hexagon(p" << level << ", " << m_radius << ", " << m_operation << ", " << m_magnitude << ");\n\n";
}

double SDFHexagon::signedDistanceInner(Vec3 p) const
{
    if (m_operation == Revolve) p = revolve2D(p, m_magnitude);
    const Vec3 k{ -0.866025404, 0.5, 0.577350269 };
    Vec2 p2D{ std::abs(p.x), std::abs(p.z) };
    p2D -= Vec2{ k.x, k.y } * std::min(Vec2{ k.x, k.y }.dot(p2D), 0.0) * 2.0;
    p2D -= Vec2{ std::clamp(p2D.x, -k.z * m_radius, k.z * m_radius), m_radius };
    double d2D = p2D.length() * (p2D.y >= 0.0 ? 1.0 : -1.0);
    return m_operation == Extrude ? extrude2D(p.y, d2D, m_magnitude * 0.5) : d2D;
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
    ss << "\td" << level << " = octagon(p" << level << ", " << m_radius << ", " << m_operation << ", " << m_magnitude << ");\n\n";
}

double SDFOctagon::signedDistanceInner(Vec3 p) const
{
    if (m_operation == Revolve) p = revolve2D(p, m_magnitude);
    const Vec3 k{ -0.9238795325, 0.3826834323, 0.4142135623 };
    Vec2 p2D{ std::abs(p.x), std::abs(p.z) };
    p2D -= Vec2{ k.x, k.y } * std::min(Vec2{ k.x, k.y }.dot(p2D), 0.0) * 2.0;
    p2D -= Vec2{ -k.x, k.y } * std::min(Vec2{ -k.x, k.y }.dot(p2D), 0.0) * 2.0;
    p2D -= Vec2{ std::clamp(p2D.x, -k.z * m_radius, k.z * m_radius), m_radius };
    double d2D = p2D.length() * (p2D.y >= 0.0 ? 1.0 : -1.0);
    return m_operation == Extrude ? extrude2D(p.y, d2D, m_magnitude * 0.5) : d2D;
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
    ss << "\td" << level << " = star(p" << level << ", " << m_radius << ", " << m_operation << ", " << m_magnitude << ", " << m_an;
    ss << ", " << Vec2{ m_asincos.y, m_asincos.x }.getShader() << ", " << Vec2{ m_esincos.y, m_esincos.x }.getShader() << ");\n\n";
}

double SDFStar::signedDistanceInner(Vec3 p) const
{
    if (m_operation == Revolve) p = revolve2D(p, m_magnitude);
    Vec2 acs{ m_asincos.y, m_asincos.x };
    Vec2 ecs{ m_esincos.y, m_esincos.x }; // ecs=vec2(0,1) for regular polygon

    Vec2 p2D{ abs(p.x), p.z };
    double bn = std::fmod(std::atan2(p2D.x, p2D.y), 2.0 * m_an) - m_an;
    Vec2 bnsincos = MF::fast_sincos(bn);
    p2D = Vec2{ bnsincos.y, std::abs(bnsincos.x) } * p2D.length();
    p2D -= acs * m_radius;
    p2D += ecs * std::clamp(-(p2D.dot(ecs)), 0.0, m_radius * acs.y / ecs.y);
    double d2D = p2D.length() * (p2D.x >= 0.0 ? 1.0 : -1.0);
    return m_operation == Extrude ? extrude2D(p.y, d2D, m_magnitude * 0.5) : d2D;
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
    ss << "\td" << level << " = triangle(p" << level << ", " << m_point1.getShader() << ", " << m_point2.getShader() << ", " << m_point3.getShader() << ", " << m_operation << ", " << m_magnitude << ");\n\n";
}

double SDFTriangle::signedDistanceInner(Vec3 p) const
{
    if (m_operation == Revolve) p = revolve2D(p, m_magnitude);
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
    return m_operation == Extrude ? extrude2D(p.y, d2D, m_magnitude * 0.5) : d2D;
}
