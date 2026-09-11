#include "SDFObject.h"
#include <algorithm>
#include <limits>
#include "Vec2.h"
#include "SDFOperation.h"

SDFObject::~SDFObject() {}

void SDFObject::update()
{
    for (auto& op : m_operations) {
        op->update();
    }
}

bool SDFObject::lit() const
{
    return m_lit;
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

double SDFObject::signedDistanceInner(Vec3 p) const
{
    return std::numeric_limits<double>::max();
}

double PlaneXZ::signedDistanceInner(Vec3 p) const
{
    return p.y - m_yPos;
}

double PlaneYZ::signedDistanceInner(Vec3 p) const
{
    return p.x - m_xPos;
}

double PlaneXY::signedDistanceInner(Vec3 p) const
{
    return p.z - m_zPos;
}

double Box::signedDistanceInner(Vec3 p) const
{
    p = p.abs();
    p -= Vec3(m_dimensions) * 0.5;
    return p.maximum(0.0).length();
}

double BoxFrame::signedDistanceInner(Vec3 p) const
{
    p = p.abs() - m_dimensions;
    Vec3 q = (p + m_edgeThickness).abs() - m_edgeThickness;
    return std::min(std::min(
        Vec3(p.x, q.y, q.z).maximum(0.0).length() + std::min(std::max(p.x, std::max(q.y, q.z)), 0.0),
        Vec3(q.x, p.y, q.z).maximum(0.0).length() + std::min(std::max(q.x, std::max(p.y, q.z)), 0.0)),
        Vec3(q.x, q.y, p.z).maximum(0.0).length() + std::min(std::max(q.x, std::max(q.y, p.z)), 0.0));
}

double Sphere::signedDistanceInner(Vec3 p) const
{
    return p.length() - m_radius;
}

double Torus::signedDistanceInner(Vec3 p) const
{
    return Vec2{ Vec2{p.x, p.z}.length() - m_radii.x, p.y }.length() - m_radii.y;
}

double CapsuleX::signedDistanceInner(Vec3 p) const
{
    p.x = p.x > 0.0 ? std::max(0.0, p.x - 0.5 * m_bodyLength) : std::min(0.0, p.x + 0.5 * m_bodyLength);
    return p.length() - m_endRadius;
}

double CapsuleY::signedDistanceInner(Vec3 p) const
{
    p.y = p.y > 0.0 ? std::max(0.0, p.y - 0.5 * m_bodyLength) : std::min(0.0, p.y + 0.5 * m_bodyLength);
    return p.length() - m_endRadius;
}

double CapsuleZ::signedDistanceInner(Vec3 p) const
{
    p.z = p.z > 0.0 ? std::max(0.0, p.z - 0.5 * m_bodyLength) : std::min(0.0, p.z + 0.5 * m_bodyLength);
    return p.length() - m_endRadius;
}

double Crystal::signedDistanceInner(Vec3 p) const
{
    p = p.abs();
    return (p.x + p.y + p.z - m_sideLength) * 0.57735027;
}
