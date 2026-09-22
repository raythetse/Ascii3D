#include "SDFObject.h"
#include <algorithm>
#include <limits>
#include "Vec2.h"
#include "SDFOperation.h"

SDFObject::SDFObject(Color color, Vec3 position, Vec3 rotationAxis, double rotations)
    : m_position{ position }, m_rotationAxis{ rotationAxis }, m_rotations{ rotations }, m_color{ color }
{
    setRotation(rotationAxis, rotations);
}

SDFObject::~SDFObject() {}

void SDFObject::update()
{
    for (auto& op : m_operations) {
        op->update();
    }
}

void SDFObject::setPosition(Vec3 position)
{
    m_position = position;
}

void SDFObject::setRotation(Vec3 axis, double rotations)
{
    m_rotationAxis = axis;
    m_rotations = rotations;
    Mat3::fromAxisAngle(m_rotationAxis, m_rotMat, m_rotations * MF::TAU);

    m_hasRotation = m_rotMat != Mat3::Identity;
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

void SDFObject::serialize(std::vector<std::string>& strVec, size_t level) const
{
    std::stringstream ss{};
    ss << std::string(level, '\t');
    ss << getClassName();
    ss << " m_position " << m_position.x << ' ' << m_position.y << ' ' << m_position.z;
    ss << " m_rotationAxis " << m_rotationAxis.x << ' ' << m_rotationAxis.y << ' ' << m_rotationAxis.z;
    ss << " m_rotations " << m_rotations;
    ss << " m_color " << m_color.x << ' ' << m_color.y << ' ' << m_color.z;
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
    p -= m_position;
    if (m_hasRotation) p = m_rotMat * p;
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
    if (level > maxLevel) {
        ss << "\t\t\tvec3 p" << level << " = p" << maxLevel << ";\n";
        ss << "\t\t\tfloat d" << level << " = d" << maxLevel << ";\n";
        maxLevel = level;
    }

    if (m_position) ss << "\t\t\tp" << level << " -= " << m_position.getShaderVec3() << ";\n";
    if (m_hasRotation) ss << "\t\t\tp" << level << " = " << m_rotMat.getShaderMat3() << " * p" << level << ";\n";
    getShaderObjectCalls(ss, level, maxLevel);

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
    ss << " m_yPos " << m_yPos;
}

void SDFPlaneXZ::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t\t\td" << level << " = abs(p" << level << ".y - " << m_yPos << ");\n";
}

double SDFPlaneXZ::signedDistanceInner(Vec3 p) const
{
    return abs(p.y - m_yPos);
}

void SDFPlaneYZ::serializeInner(std::stringstream& ss) const
{
    ss << " m_xPos " << m_xPos;
}

void SDFPlaneYZ::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t\t\td" << level << " = abs(p" << level << ".x - " << m_xPos << ");\n";
}

double SDFPlaneYZ::signedDistanceInner(Vec3 p) const
{
    return abs(p.x - m_xPos);
}

void SDFPlaneXY::serializeInner(std::stringstream& ss) const
{
    ss << " m_zPos " << m_zPos;
}

void SDFPlaneXY::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t\t\td" << level << " = abs(p" << level << ".z - " << m_zPos << ");\n";
}

double SDFPlaneXY::signedDistanceInner(Vec3 p) const
{
    return abs(p.z - m_zPos);
}

void SDFBox::serializeInner(std::stringstream& ss) const
{
    ss << " m_dimensions " << m_dimensions.x << ' ' << m_dimensions.y << ' ' << m_dimensions.z;
}

void SDFBox::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t\t\td" << level << " = box(p" << level << ", " << m_dimensions.getShaderVec3() << ");\n\n";
}

double SDFBox::signedDistanceInner(Vec3 p) const
{
    p = p.abs();
    p -= Vec3(m_dimensions) * 0.5;
    return p.maximum(0.0).length() + std::min(std::max(p.x, std::max(p.y, p.z)), 0.0);
}

void SDFBoxFrame::serializeInner(std::stringstream& ss) const
{
    ss << " m_dimensions " << m_dimensions.x << ' ' << m_dimensions.y << ' ' << m_dimensions.z;
    ss << " m_edgeThickness " << m_edgeThickness;
}

void SDFBoxFrame::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t\t\td" << level << " = boxFrame(p" << level << ", " << m_dimensions.getShaderVec3() << ", " << m_edgeThickness << ");\n\n";
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
    ss << " m_radius " << m_radius;
}

void SDFSphere::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t\t\td" << level << " = sphere(p" << level << ", " << m_radius << ");\n\n";
}

double SDFSphere::signedDistanceInner(Vec3 p) const
{
    return p.length() - m_radius;
}

void SDFTorus::serializeInner(std::stringstream& ss) const
{
    ss << " m_radii " << m_radii.x << ' ' << m_radii.y;
}

void SDFTorus::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t\t\td" << level << " = torus(p" << level << ", " << m_radii.getShaderVec2() << ");\n\n";
}

double SDFTorus::signedDistanceInner(Vec3 p) const
{
    return Vec2{ Vec2{p.x, p.z}.length() - m_radii.x, p.y }.length() - m_radii.y;
}

void SDFCapsuleX::serializeInner(std::stringstream& ss) const
{
    ss << " m_radius " << m_radius;
    ss << " m_bodyLength " << m_bodyLength;
}

void SDFCapsuleX::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t\t\td" << level << " = capsuleX(p" << level << ", " << m_bodyLength << ", " << m_radius << ");\n\n";
}

double SDFCapsuleX::signedDistanceInner(Vec3 p) const
{
    p.x = p.x > 0.0 ? std::max(0.0, p.x - 0.5 * m_bodyLength) : std::min(0.0, p.x + 0.5 * m_bodyLength);
    return p.length() - m_radius;
}

void SDFCapsuleY::serializeInner(std::stringstream& ss) const
{
    ss << " m_radius " << m_radius;
    ss << " m_bodyLength " << m_bodyLength;
}

void SDFCapsuleY::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t\t\td" << level << " = capsuleY(p" << level << ", " << m_bodyLength << ", " << m_radius << ");\n\n";
}

double SDFCapsuleY::signedDistanceInner(Vec3 p) const
{
    p.y = p.y > 0.0 ? std::max(0.0, p.y - 0.5 * m_bodyLength) : std::min(0.0, p.y + 0.5 * m_bodyLength);
    return p.length() - m_radius;
}

void SDFCapsuleZ::serializeInner(std::stringstream& ss) const
{
    ss << " m_radius " << m_radius;
    ss << " m_bodyLength " << m_bodyLength;
}

void SDFCapsuleZ::getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const
{
    ss << "\t\t\td" << level << " = capsuleZ(p" << level << ", " << m_bodyLength << ", " << m_radius << ");\n\n";
}

double SDFCapsuleZ::signedDistanceInner(Vec3 p) const
{
    p.z = p.z > 0.0 ? std::max(0.0, p.z - 0.5 * m_bodyLength) : std::min(0.0, p.z + 0.5 * m_bodyLength);
    return p.length() - m_radius;
}

void SDFCrystal::serializeInner(std::stringstream& ss) const
{
    ss << " m_edgeLength " << m_edgeLength;
}

double SDFCrystal::signedDistanceInner(Vec3 p) const
{
    p = p.abs();
    return (p.x + p.y + p.z - m_edgeLength) * 0.57735027;
}

void SDFCylinderX::serializeInner(std::stringstream& ss) const
{
    ss << " m_sideLength " << m_sideLength;
    ss << " m_radius " << m_radius;
}

double SDFCylinderX::signedDistanceInner(Vec3 p) const
{
    Vec2 yz{ p.y, p.z };
    Vec2 d = Vec2{ yz.length(), p.x }.abs() - Vec2(m_radius, m_sideLength);
    return std::min(std::max(d.x, d.y), 0.0) + d.maximum(0.0).length();
}

void SDFCylinderY::serializeInner(std::stringstream& ss) const
{
    ss << " m_sideLength " << m_sideLength;
    ss << " m_radius " << m_radius;
}

double SDFCylinderY::signedDistanceInner(Vec3 p) const
{
    Vec2 xz{ p.x, p.z };
    Vec2 d = Vec2{ xz.length(), p.y }.abs() - Vec2(m_radius, m_sideLength);
    return std::min(std::max(d.x, d.y), 0.0) + d.maximum(0.0).length();
}

void SDFCylinderZ::serializeInner(std::stringstream& ss) const
{
    ss << " m_sideLength " << m_sideLength;
    ss << " m_radius " << m_radius;
}

double SDFCylinderZ::signedDistanceInner(Vec3 p) const
{
    Vec2 xy{ p.x, p.y };
    Vec2 d = Vec2{ xy.length(), p.z }.abs() - Vec2(m_radius, m_sideLength);
    return std::min(std::max(d.x, d.y), 0.0) + d.maximum(0.0).length();
}

void SDFChainLink::serializeInner(std::stringstream& ss) const
{
    ss << " m_sideLength " << m_sideLength;
    ss << " m_radii " << m_radii.x << ' ' << m_radii.y;
}

double SDFChainLink::signedDistanceInner(Vec3 p) const
{
    p.x = p.x > 0.0 ? std::max(0.0, p.x - m_sideLength * 0.5) : std::min(0.0, p.x + m_sideLength * 0.5);
    return Vec2{ Vec2{p.x, p.z}.length() - m_radii.x, p.y }.length() - m_radii.y;
}

void SDFTube::serializeInner(std::stringstream& ss) const
{
    ss << " m_height " << m_height;
    ss << " m_radii " << m_radii.x << ' ' << m_radii.y;
}

double SDFTube::signedDistanceInner(Vec3 p) const
{
    p.y = p.y > 0.0 ? std::max(0.0, p.y - m_height * 0.5) : std::min(0.0, p.y + m_height * 0.5);
    return Vec2{ Vec2{p.x, p.z}.length() - m_radii.x, p.y }.length() - m_radii.y;
}
