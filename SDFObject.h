#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include "Mat3.h"
#include "Vec2.h"
#include "Vec3.h"

class SDFOperation;

// Scene objects use 'Signed-Distance Fields' (SDFs) to build out a scene
// For more information on SDFs: https://iquilezles.org/articles/distfunctions/
// Most (if not all) SDF functions are adapted from this article.
class SDFObject
{
public:
protected:
	size_t m_objId;
	Vec3 m_position{};
	Mat3 m_rotationMat{};
	Color m_color;
	bool m_lit;
	std::vector<std::unique_ptr<SDFOperation>> m_operations;
	virtual double signedDistanceInner(Vec3 p) const;
public:
	SDFObject(Vec3 position = Vec3::Zero, Vec3 rotation = Vec3::Zero, Color color = Color::White, bool lit = true);
	~SDFObject();
	Vec3 position() const;
	virtual double signedDistance(Vec3 p, Color* colorOut = nullptr) const;
	bool lit() const;
	void pushOperation(std::unique_ptr<SDFOperation> operation);
	template <typename T, typename... Args>
	SDFObject* pushOperation(Args&&... args);
	void setColor(Color color);
};

template <typename T, typename... Args>
SDFObject* SDFObject::pushOperation(Args&&... args) {
	m_operations.push_back(std::make_unique<T>(std::forward<Args>(args)...));
	return this;
}

class PlaneXZ : public SDFObject
{
public:
	PlaneXZ(double yPos = 0.0, Color color = Color::White, bool lit = true)
		: SDFObject(Vec3::Up * yPos, Vec3::Zero, color, lit) {}
	double signedDistanceInner(Vec3 p) const override;
};

class PlaneYZ : public SDFObject
{
public:
	PlaneYZ(double xPos = 0.0, Color color = Color::White, bool lit = true)
		: SDFObject(Vec3::Right* xPos, Vec3::Zero, color, lit) {}
	double signedDistanceInner(Vec3 p) const override;
};

class PlaneXY : public SDFObject
{
public:
	PlaneXY(double zPos = 0.0, Color color = Color::White, bool lit = true)
		: SDFObject(Vec3::Forward* zPos, Vec3::Zero, color, lit) {}
	double signedDistanceInner(Vec3 p) const override;
};

class Box : public SDFObject
{
private:
	Vec3 m_dimensions;
public:
	Box(Vec3 position = Vec3::Zero, Vec3 rotation = Vec3::Zero, Vec3 dimensions = Vec3::One, Color color = Color::White, bool lit = true)
		: SDFObject(position, rotation, color, lit), m_dimensions(dimensions) {}
	double signedDistanceInner(Vec3 p) const override;
};

class BoxFrame : public SDFObject
{
private:
	Vec3 m_dimensions;
	double m_edgeThickness;
public:
	BoxFrame(Vec3 position = Vec3::Zero, Vec3 rotation = Vec3::Zero, Vec3 dimensions = Vec3::One, double edgeThickness = 0.1, Color color = Color::White, bool lit = true)
		: SDFObject(position, rotation, color, lit), m_dimensions(dimensions), m_edgeThickness(edgeThickness) {}
	double signedDistanceInner(Vec3 p) const override;
};

class Sphere : public SDFObject
{
private:
	double m_radius;
public:
	Sphere(Vec3 position = Vec3::Zero, double radius = 0.5, Color color = Color::White, bool lit = true);
	double signedDistanceInner(Vec3 p) const override;
};

class Torus : public SDFObject
{
private:
	Vec2 m_radii;
public:
	Torus(Vec3 position = Vec3::Zero, Vec3 rotation = Vec3::Zero, Vec2 radii = Vec2{ 1.0, 0.25 }, Color color = Color::White, bool lit = true)
		: SDFObject(position, rotation, color, lit), m_radii(radii) {}
	double signedDistanceInner(Vec3 p) const override;
};

