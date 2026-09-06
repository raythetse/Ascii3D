#include "SceneObject.h"
#include <algorithm>
#include <limits>
#include "Vec2.h"
#include "Operation.h"

SceneObject::SceneObject(
    Vec3 position,
    Vec3 rotation,
    Vec3 color,
    double rounding,
    bool lit
)
    : m_startPosition(position)
    , m_currentPosition(position)
    , m_color(color)
    , m_rounding(rounding)
    , m_lit(lit)
{
    if (rotation) {
        m_rotationMat = Mat3::fromAxisAngle(rotation.getNormal(), rotation.length());
    }
    else {
        m_rotationMat = Mat3::Identity;
    }
}

void SceneObject::update(double time)
{
    m_currentPosition = m_startPosition;
}

SceneObject::~SceneObject()
{}

bool SceneObject::lit() const
{
    return m_lit;
}

void SceneObject::pushOperation(std::unique_ptr<SDFOp::SDFOperation> operation)
{
    m_operations.push_back(std::move(operation));
}

Vec3 SceneObject::position() const
{
    return m_currentPosition;
}

double SceneObject::signedDistance(Vec3 p, double time, Vec3* colorOut) const
{
    double distance = std::numeric_limits<double>::max();
    for (auto& op : m_operations) {
        op->execute(distance, p, time, m_color, colorOut);
    }
    if (colorOut && m_operations.size() == 0) {
        *colorOut = m_color;
    }
    return distance;
}

double VisualObject::signedDistance(Vec3 p, double time, Vec3* colorOut) const
{
    double distance{ signedDistanceInner(p, time) };
    for (auto& op : m_operations) {
        op->execute(distance, p, time, m_color, colorOut);
    }
    if (colorOut && m_operations.size() == 0) {
        *colorOut = m_color;
    }
    return distance;
}

Box::Box(Vec3 position, Vec3 rotation, Vec3 dimensions, Vec3 color, double rounding, bool lit)
    : VisualObject(position, rotation, color, rounding, lit), m_dimensions(dimensions) {}

double Box::signedDistanceInner(Vec3 p, double time) const
{
    p -= m_currentPosition;
    p = m_rotationMat * p;
    p = abs(p);
    p -= Vec3(m_dimensions) * 0.5;
    return p.maximum(0.0).length() - m_rounding;
}

Sphere::Sphere(Vec3 position, double radius, Vec3 color, bool lit)
    : VisualObject(position, Vec3::Zero, color, radius, lit) {}

double Sphere::signedDistanceInner(Vec3 p, double time) const
{
    return p.distance(m_currentPosition) - m_rounding;
}

Torus::Torus(Vec3 position, Vec3 rotation, Vec2 radii, Vec3 color, bool lit)
    : VisualObject(position, rotation, color, 0.0, lit), m_radii(radii) {}

double Torus::signedDistanceInner(Vec3 p, double time) const
{
    p -= m_currentPosition;
    p = m_rotationMat * p;
    return Vec2{ Vec2{p.x, p.z}.length() - m_radii.x, p.y }.length() - m_radii.y;
}
