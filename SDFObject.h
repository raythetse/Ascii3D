#pragma once
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include "SDFDefaults.h"
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
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const {};
public:
	SDFObject(Color color = DEF::OBJColor, Vec3 position = DEF::ObjPosition, Vec3 rotationAxis = DEF::OBJRotAxis, double rotations = DEF::OBJRotations);
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
	void getShaderCalls(std::stringstream& ss, size_t level, size_t& maxLevel);
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
	void serializeInner(std::stringstream& ss) const override;
public:
	SDFPlaneXZ(double yPos = DEF::OBJPlanePos, Color color = DEF::OBJColor)
		: SDFObject{ color }, m_yPos{ yPos } {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFPlaneYZ : public SDFObject
{
private:
	double m_xPos;
protected:
	std::string getClassName() const override { return "SDFPlaneYZ"; }
	void serializeInner(std::stringstream& ss) const override;
public:
	SDFPlaneYZ(double xPos = DEF::OBJPlanePos, Color color = DEF::OBJColor)
		: SDFObject{ color }, m_xPos(xPos) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFPlaneXY : public SDFObject
{
private:
	double m_zPos;
protected:
	std::string getClassName() const override { return "SDFPlaneXY"; }
	void serializeInner(std::stringstream& ss) const override;
public:
	SDFPlaneXY(double zPos = DEF::OBJPlanePos, Color color = DEF::OBJColor)
		: SDFObject{ color }, m_zPos(zPos) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFBox : public SDFObject
{
private:
	Vec3 m_dimensions;
protected:
	std::string getClassName() const override { return "SDFBox"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFBox(Vec3 dimensions = DEF::OBJBoxDimensions, Color color = DEF::OBJColor, Vec3 position = DEF::ObjPosition, Vec3 rotationAxis = DEF::OBJRotAxis, double rotations = DEF::OBJRotations)
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
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFBoxFrame(Vec3 dimensions = DEF::OBJBoxDimensions, double edgeThickness = DEF::OBJFrameThickness, Color color = DEF::OBJColor, Vec3 position = DEF::ObjPosition, Vec3 rotationAxis = DEF::OBJRotAxis, double rotations = DEF::OBJRotations)
		: SDFObject(color, position, rotationAxis, rotations), m_dimensions(dimensions), m_edgeThickness(edgeThickness) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFSphere : public SDFObject
{
private:
	double m_radius;
protected:
	std::string getClassName() const override { return "SDFSphere"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFSphere(double radius = DEF::OBJRadius, Color color = DEF::OBJColor, Vec3 position = DEF::ObjPosition, Vec3 rotationAxis = DEF::OBJRotAxis, double rotations = DEF::OBJRotations)
		: SDFObject(color, position, rotationAxis, rotations), m_radius(radius) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFTorus : public SDFObject
{
private:
	Vec2 m_radii;
protected:
	std::string getClassName() const override { return "SDFTorus"; }
	void serializeInner(std::stringstream& ss) const override;
public:
	SDFTorus(Vec2 radii = DEF::OBJRadii, Color color = DEF::OBJColor, Vec3 position = DEF::ObjPosition, Vec3 rotationAxis = DEF::OBJRotAxis, double rotations = DEF::OBJRotations)
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
	void serializeInner(std::stringstream& ss) const override;
public:
	SDFCapsuleX(double radius = DEF::OBJRadius, double bodyLength = DEF::OBJLength, Color color = DEF::OBJColor, Vec3 position = DEF::ObjPosition, Vec3 rotationAxis = DEF::OBJRotAxis, double rotations = DEF::OBJRotations)
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
	void serializeInner(std::stringstream& ss) const override;
public:
	SDFCapsuleY(double radius = DEF::OBJRadius, double bodyLength = DEF::OBJLength, Color color = DEF::OBJColor, Vec3 position = DEF::ObjPosition, Vec3 rotationAxis = DEF::OBJRotAxis, double rotations = DEF::OBJRotations)
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
	void serializeInner(std::stringstream& ss) const override;
public:
	SDFCapsuleZ(double radius = DEF::OBJRadius, double bodyLength = DEF::OBJLength, Color color = DEF::OBJColor, Vec3 position = DEF::ObjPosition, Vec3 rotationAxis = DEF::OBJRotAxis, double rotations = DEF::OBJRotations)
		: SDFObject(color, position, rotationAxis, rotations), m_radius(radius), m_bodyLength(bodyLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFCrystal : public SDFObject
{
private:
	double m_edgeLength;
protected:
	std::string getClassName() const override { return "SDFCrystal"; }
	void serializeInner(std::stringstream& ss) const override;
public:
	SDFCrystal(double edgeLength = DEF::OBJLength, Color color = DEF::OBJColor, Vec3 position = DEF::ObjPosition, Vec3 rotationAxis = DEF::OBJRotAxis, double rotations = DEF::OBJRotations)
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
	void serializeInner(std::stringstream& ss) const override;
public:
	SDFCylinderX(double radius = DEF::OBJRadius, double sideLength = DEF::OBJLength, Color color = DEF::OBJColor, Vec3 position = DEF::ObjPosition, Vec3 rotationAxis = DEF::OBJRotAxis, double rotations = DEF::OBJRotations)
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
	void serializeInner(std::stringstream& ss) const override;
public:
	SDFCylinderY(double radius = DEF::OBJRadius, double sideLength = DEF::OBJLength, Color color = DEF::OBJColor, Vec3 position = DEF::ObjPosition, Vec3 rotationAxis = DEF::OBJRotAxis, double rotations = DEF::OBJRotations)
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
	void serializeInner(std::stringstream& ss) const override;
public:
	SDFCylinderZ(double radius = DEF::OBJRadius, double sideLength = DEF::OBJLength, Color color = DEF::OBJColor, Vec3 position = DEF::ObjPosition, Vec3 rotationAxis = DEF::OBJRotAxis, double rotations = DEF::OBJRotations)
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
	void serializeInner(std::stringstream& ss) const override;
public:
	SDFChainLink(double sideLength = DEF::OBJLength, Vec2 radii = DEF::OBJRadii, Color color = DEF::OBJColor, Vec3 position = DEF::ObjPosition, Vec3 rotationAxis = DEF::OBJRotAxis, double rotations = DEF::OBJRotations)
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
	void serializeInner(std::stringstream& ss) const override;
public:
	SDFTube(double height = DEF::OBJLength, Vec2 radii = DEF::OBJRadii, Color color = DEF::OBJColor, Vec3 position = DEF::ObjPosition, Vec3 rotationAxis = DEF::OBJRotAxis, double rotations = DEF::OBJRotations)
		: SDFObject(color, position, rotationAxis, rotations), m_height(height), m_radii(radii) {}
	double signedDistanceInner(Vec3 p) const override;
};

