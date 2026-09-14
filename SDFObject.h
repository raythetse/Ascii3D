#pragma once
#include <iostream>
#include <memory>
#include <sstream>
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
	virtual void serializeInner(std::stringstream& ss) const {};
	virtual std::string getClassName() const { return "SDFObject"; }
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
	virtual void serialize(std::vector<std::string> &strVec, size_t level) const;
};

template <typename T, typename... Args>
SDFOperation* SDFObject::pushOperation(Args&&... args) {
	m_operations.push_back(std::make_unique<T>(std::forward<Args>(args)...));
	return m_operations.back().get();
}

class SDFPlaneXZ : public SDFObject
{
private:
	double m_yPos;
protected:
	std::string getClassName() const override { return "SDFPlaneXZ"; }
public:
	SDFPlaneXZ(double yPos = 0.0, Color color = Color::White)
		: SDFObject{ color }, m_yPos{ yPos } {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFPlaneYZ : public SDFObject
{
private:
	double m_xPos;
protected:
	std::string getClassName() const override { return "SDFPlaneYZ"; }
public:
	SDFPlaneYZ(double xPos = 0.0, Color color = Color::White)
		: SDFObject{ color }, m_xPos(xPos) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFPlaneXY : public SDFObject
{
private:
	double m_zPos;
protected:
	std::string getClassName() const override { return "SDFPlaneXY"; }
public:
	SDFPlaneXY(double zPos = 0.0, Color color = Color::White)
		: SDFObject{ color }, m_zPos(zPos) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFBox : public SDFObject
{
private:
	Vec3 m_dimensions;
protected:
	std::string getClassName() const override { return "SDFBox"; }
public:
	SDFBox(Vec3 dimensions = Vec3::One, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_dimensions(dimensions) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFBoxFrame : public SDFObject
{
private:
	Vec3 m_dimensions;
	double m_edgeThickness;
protected:
	std::string getClassName() const override { return "SDFBoxFrame"; }
public:
	SDFBoxFrame(Vec3 dimensions = Vec3::One, double edgeThickness = 0.1, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_dimensions(dimensions), m_edgeThickness(edgeThickness) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFSphere : public SDFObject
{
private:
	double m_radius;
protected:
	std::string getClassName() const override { return "SDFSphere"; }
public:
	SDFSphere(double radius = 0.5, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_radius(radius) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFTorus : public SDFObject
{
private:
	Vec2 m_radii;
protected:
	std::string getClassName() const override { return "SDFTorus"; }
public:
	SDFTorus(Vec2 radii = Vec2{ 1.0, 0.25 }, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_radii(radii) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFCapsuleX : public SDFObject
{
private:
	double m_radius;
	double m_bodyLength;
protected:
	std::string getClassName() const override { return "SDFCapsuleX"; }
public:
	SDFCapsuleX(double radius = 0.5, double bodyLength = 1.0, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_radius(radius), m_bodyLength(bodyLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFCapsuleY : public SDFObject
{
private:
	double m_radius;
	double m_bodyLength;
protected:
	std::string getClassName() const override { return "SDFCapsuleY"; }
public:
	SDFCapsuleY(double radius = 0.5, double bodyLength = 1.0, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_radius(radius), m_bodyLength(bodyLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFCapsuleZ : public SDFObject
{
private:
	double m_radius;
	double m_bodyLength;
protected:
	std::string getClassName() const override { return "SDFCapsuleZ"; }
public:
	SDFCapsuleZ(double radius = 0.5, double bodyLength = 1.0, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_radius(radius), m_bodyLength(bodyLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFCrystal : public SDFObject
{
private:
	double m_edgeLength;
protected:
	std::string getClassName() const override { return "SDFCrystal"; }
public:
	SDFCrystal(double edgeLength = 1.0, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_edgeLength(edgeLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFCylinderX : public SDFObject
{
private:
	double m_sideLength;
	double m_radius;
protected:
	std::string getClassName() const override { return "SDFCylinderX"; }
public:
	SDFCylinderX(double sideLength = 1.0, double radius = 0.5, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_sideLength(sideLength), m_radius(radius) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFCylinderY : public SDFObject
{
private:
	double m_sideLength;
	double m_radius;
protected:
	std::string getClassName() const override { return "SDFCylinderY"; }
public:
	SDFCylinderY(double sideLength = 1.0, double radius = 0.5, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_sideLength(sideLength), m_radius(radius) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFCylinderZ : public SDFObject
{
private:
	double m_sideLength;
	double m_radius;
protected:
	std::string getClassName() const override { return "SDFCylinderZ"; }
public:
	SDFCylinderZ(double sideLength = 1.0, double radius = 0.5, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_sideLength(sideLength), m_radius(radius) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFChainLink : public SDFObject
{
private:
	double m_sideLength;
	Vec2 m_radii;
protected:
	std::string getClassName() const override { return "SDFChainLink"; }
public:
	SDFChainLink(double sideLength = 1.0, Vec2 radii = {1.0, 0.25}, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_sideLength(sideLength), m_radii(radii) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFTube : public SDFObject
{
private:
	double m_height;
	Vec2 m_radii;
protected:
	std::string getClassName() const override { return "SDFTube"; }
public:
	SDFTube(double height = 1.0, Vec2 radii = { 1.0, 0.25 }, Color color = Color::White, Vec3 position = {}, Vec3 rotationAxis = Vec3::Up, double rotations = 0.0)
		: SDFObject(color, position, rotationAxis, rotations), m_height(height), m_radii(radii) {}
	double signedDistanceInner(Vec3 p) const override;
};

