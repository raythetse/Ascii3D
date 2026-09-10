#include "SDFObject.h"
#include <algorithm>
#include <limits>
#include "Vec2.h"
#include "SDFOperation.h"

SDFObject::SDFObject(Vec3 position, Vec3 rotation, Color color, bool lit)
    : m_position(position), m_color(color), m_lit(lit)
{
    static size_t id{};
    m_objId = id++;

    if (rotation) {
        Mat3::fromAxisAngle(rotation.getNormal(), m_rotationMat, rotation.length() * MF::TAU);
    }
    else {
        m_rotationMat = Mat3::Identity;
    }
}

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

Vec3 SDFObject::position() const
{
    return m_position;
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
    return p.y - m_position.y;
}

double PlaneYZ::signedDistanceInner(Vec3 p) const
{
    return p.x - m_position.x;
}

double PlaneXY::signedDistanceInner(Vec3 p) const
{
    return p.z - m_position.z;
}

double Box::signedDistanceInner(Vec3 p) const
{
    p -= m_position;
    p = m_rotationMat * p;
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

Sphere::Sphere(Vec3 position, double radius, Color color, bool lit)
    : SDFObject(position, Vec3::Zero, color, lit), m_radius(radius) {}

double Sphere::signedDistanceInner(Vec3 p) const
{
    return p.distance(m_position) - m_radius;
}

double Torus::signedDistanceInner(Vec3 p) const
{
    p -= m_position;
    p = m_rotationMat * p;
    return Vec2{ Vec2{p.x, p.z}.length() - m_radii.x, p.y }.length() - m_radii.y;
}
