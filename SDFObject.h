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
	Vec3 m_position{};
	Vec3 m_rotationAxis{};
	double m_rotations{};
	Mat3 m_rotMat{};
	bool m_hasRotation{};
	Color m_color{};
	std::vector<std::unique_ptr<SDFOperation>> m_operations;
	virtual double signedDistanceInner(Vec3 p) const;
public:
	SDFObject(Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0);
	~SDFObject();
	void update();
	void setPosition(Vec3 position);
	void setRotation(Vec3 axis, double rotations);
	virtual double signedDistance(Vec3 p, Color* colorOut = nullptr) const;
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
	PlaneXZ(double yPos = 0.0, Color color = Color::White)
		: SDFObject{ color }, m_yPos{ yPos } {}
	double signedDistanceInner(Vec3 p) const override;
};

class PlaneYZ : public SDFObject
{
	double m_xPos;
public:
	PlaneYZ(double xPos = 0.0, Color color = Color::White)
		: SDFObject{ color }, m_xPos(xPos) {}
	double signedDistanceInner(Vec3 p) const override;
};

class PlaneXY : public SDFObject
{
	double m_zPos;
public:
	PlaneXY(double zPos = 0.0, Color color = Color::White)
		: SDFObject{ color }, m_zPos(zPos) {}
	double signedDistanceInner(Vec3 p) const override;
};

class Box : public SDFObject
{
private:
	Vec3 m_dimensions;
public:
	Box(Vec3 dimensions = Vec3::One, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_dimensions(dimensions) {}
	double signedDistanceInner(Vec3 p) const override;
};

class BoxFrame : public SDFObject
{
private:
	Vec3 m_dimensions;
	double m_edgeThickness;
public:
	BoxFrame(Vec3 dimensions = Vec3::One, double edgeThickness = 0.1, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_dimensions(dimensions), m_edgeThickness(edgeThickness) {}
	double signedDistanceInner(Vec3 p) const override;
};

class Sphere : public SDFObject
{
private:
	double m_radius;
public:
	Sphere(double radius = 0.5, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_radius(radius) {}
	double signedDistanceInner(Vec3 p) const override;
};

class Torus : public SDFObject
{
private:
	Vec2 m_radii;
public:
	Torus(Vec2 radii = Vec2{ 1.0, 0.25 }, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_radii(radii) {}
	double signedDistanceInner(Vec3 p) const override;
};

class CapsuleX : public SDFObject
{
private:
	double m_radius;
	double m_bodyLength;
public:
	CapsuleX(double radius = 0.5, double bodyLength = 1.0, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_radius(radius), m_bodyLength(bodyLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

class CapsuleY : public SDFObject
{
private:
	double m_radius;
	double m_bodyLength;
public:
	CapsuleY(double radius = 0.5, double bodyLength = 1.0, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_radius(radius), m_bodyLength(bodyLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

class CapsuleZ : public SDFObject
{
private:
	double m_radius;
	double m_bodyLength;
public:
	CapsuleZ(double radius = 0.5, double bodyLength = 1.0, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_radius(radius), m_bodyLength(bodyLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

class Crystal : public SDFObject
{
private:
	double m_edgeLength;
public:
	Crystal(double edgeLength = 1.0, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_edgeLength(edgeLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

class CrystalLong : public SDFObject
{
private:
	double m_edgeLength;
	double m_sideLength;
public:
	CrystalLong(double edgeLength = 1.0, double sideLength = 1.0, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_edgeLength(edgeLength), m_sideLength(sideLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

class CylinderX : public SDFObject
{
private:
	double m_sideLength;
	double m_radius;
public:
	CylinderX(double sideLength = 1.0, double radius = 0.5, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_sideLength(sideLength), m_radius(radius) {}
	double signedDistanceInner(Vec3 p) const override;
};

class CylinderY : public SDFObject
{
private:
	double m_sideLength;
	double m_radius;
public:
	CylinderY(double sideLength = 1.0, double radius = 0.5, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_sideLength(sideLength), m_radius(radius) {}
	double signedDistanceInner(Vec3 p) const override;
};

class CylinderZ : public SDFObject
{
private:
	double m_sideLength;
	double m_radius;
public:
	CylinderZ(double sideLength = 1.0, double radius = 0.5, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_sideLength(sideLength), m_radius(radius) {}
	double signedDistanceInner(Vec3 p) const override;
};

class ChainLink : public SDFObject
{
private:
	double m_sideLength;
	Vec2 m_radii;
public:
	ChainLink(double sideLength = 1.0, Vec2 radii = {1.0, 0.25}, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_sideLength(sideLength), m_radii(radii) {}
	double signedDistanceInner(Vec3 p) const override;
};

class Tube : public SDFObject
{
private:
	double m_height;
	Vec2 m_radii;
public:
	Tube(double height = 1.0, Vec2 radii = { 1.0, 0.25 }, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_height(height), m_radii(radii) {}
	double signedDistanceInner(Vec3 p) const override;
};

