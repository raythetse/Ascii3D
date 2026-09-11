#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include "Mat3.h"
#include "Vec2.h"
#include "Vec3.h"
#include "MathFunctions.h"
#include "SDFOperation.h"

class SDFOperation;

// SDFObjects use 'Signed-Distance Fields' (SDFs) to build out a scene
// For more information on SDFs: https://iquilezles.org/articles/distfunctions/
// Many SDF functions are adapted from the above article.
class SDFObject
{
public:
protected:
	Color m_color;
	bool m_lit;
	std::vector<std::unique_ptr<SDFOperation>> m_operations;
	virtual double signedDistanceInner(Vec3 p) const;
public:
	SDFObject(Color color = Color::White, bool lit = true)
		: m_color(color), m_lit(lit) {}
	~SDFObject();
	void update();
	virtual double signedDistance(Vec3 p, Color* colorOut = nullptr) const;
	bool lit() const;
	SDFOperation* pushOperation(std::unique_ptr<SDFOperation> operation);
	template <typename T, typename... Args>
	SDFOperation* pushOperation(Args&&... args);
	void setColor(Color color);
};

template <typename T, typename... Args>
SDFOperation* SDFObject::pushOperation(Args&&... args) {
	m_operations.push_back(std::make_unique<T>(std::forward<Args>(args)...));
	return m_operations.back().get();
}

class PlaneXZ : public SDFObject
{
private:
	double m_yPos;
public:
	PlaneXZ(double yPos = 0.0, Color color = Color::White, bool lit = true)
		: SDFObject{ color, lit }, m_yPos{ yPos } {}
	double signedDistanceInner(Vec3 p) const override;
};

class PlaneYZ : public SDFObject
{
	double m_xPos;
public:
	PlaneYZ(double xPos = 0.0, Color color = Color::White, bool lit = true)
		: SDFObject{ color, lit }, m_xPos(xPos) {}
	double signedDistanceInner(Vec3 p) const override;
};

class PlaneXY : public SDFObject
{
	double m_zPos;
public:
	PlaneXY(double zPos = 0.0, Color color = Color::White, bool lit = true)
		: SDFObject{ color, lit }, m_zPos(zPos) {}
	double signedDistanceInner(Vec3 p) const override;
};

class Box : public SDFObject
{
private:
	Vec3 m_dimensions;
public:
	Box(Vec3 dimensions = Vec3::One, Color color = Color::White, bool lit = true)
		: SDFObject(color, lit), m_dimensions(dimensions) {}
	double signedDistanceInner(Vec3 p) const override;
};

class BoxFrame : public SDFObject
{
private:
	Vec3 m_dimensions;
	double m_edgeThickness;
public:
	BoxFrame(Vec3 dimensions = Vec3::One, double edgeThickness = 0.1, Color color = Color::White, bool lit = true)
		: SDFObject(color, lit), m_dimensions(dimensions), m_edgeThickness(edgeThickness) {}
	double signedDistanceInner(Vec3 p) const override;
};

class Sphere : public SDFObject
{
private:
	double m_radius;
public:
	Sphere(double radius = 0.5, Color color = Color::White, bool lit = true)
		: SDFObject(color, lit), m_radius(radius) {}
	double signedDistanceInner(Vec3 p) const override;
};

class Torus : public SDFObject
{
private:
	Vec2 m_radii;
public:
	Torus(Vec2 radii = Vec2{ 1.0, 0.25 }, Color color = Color::White, bool lit = true)
		: SDFObject(color, lit), m_radii(radii) {}
	double signedDistanceInner(Vec3 p) const override;
};

class CapsuleX : public SDFObject
{
private:
	double m_endRadius;
	double m_bodyLength;
public:
	CapsuleX(double endRadius = 0.5, double bodyLength = 1.0, Color color = Color::White, bool lit = true)
		: SDFObject(color, lit), m_endRadius(endRadius), m_bodyLength(bodyLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

class CapsuleY : public SDFObject
{
private:
	double m_endRadius;
	double m_bodyLength;
public:
	CapsuleY(double endRadius = 0.5, double bodyLength = 1.0, Color color = Color::White, bool lit = true)
		: SDFObject(color, lit), m_endRadius(endRadius), m_bodyLength(bodyLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

class CapsuleZ : public SDFObject
{
private:
	double m_endRadius;
	double m_bodyLength;
public:
	CapsuleZ(double endRadius = 0.5, double bodyLength = 1.0, Color color = Color::White, bool lit = true)
		: SDFObject(color, lit), m_endRadius(endRadius), m_bodyLength(bodyLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

class Crystal : public SDFObject
{
private:
	double m_sideLength;
public:
	Crystal(double sideLength = 1.0, Color color = Color::White, bool lit = true)
		: SDFObject(color, lit), m_sideLength(sideLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

class CylinderX : public SDFObject
{
private:
	double m_sideLength;
public:
	CylinderX(double sideLength = 1.0, Color color = Color::White, bool lit = true)
		: SDFObject(color, lit), m_sideLength(sideLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

