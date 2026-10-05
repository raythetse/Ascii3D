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
// Most SDF functions are adapted from the above article.
class SDFObject
{
public:
	enum OP2D
	{
		Extrude,
		Revolve,
		None,
	};
protected:
	Color m_color{};
	std::vector<std::unique_ptr<SDFOperation>> m_operations;
	virtual double signedDistanceInner(Vec3 p) const;
	virtual void serializeInner(std::stringstream& ss) const {};
	virtual std::string getClassName() const { return "SDFObject"; }
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const {};
	double extrude2D(double pDotY, double d2D, double magnitude) const;
	Vec3 revolve2D(Vec3 p, double radius) const;
public:
	SDFObject(Color color = Color::White)
		: m_color{ color } {}
	~SDFObject();
	void update();
	void setPosition(Vec3 position);
	void setRotation(Vec3 axis, double rotations);
	virtual double signedDistance(Vec3 p, Color* colorOut = nullptr) const;
	SDFOperation* pushOperation(std::unique_ptr<SDFOperation> operation);
	template <typename T, typename... Args>
	SDFOperation* pushOperation(Args&&... args);
	void setColor(Color color);
	Color getColor();
	virtual void serialize(std::vector<std::string> &strVec, size_t level) const;
	void getShaderCalls(std::stringstream& ss, size_t level, size_t& maxLevel);
};

template <typename T, typename... Args>
SDFOperation* SDFObject::pushOperation(Args&&... args) {
	m_operations.push_back(std::make_unique<T>(std::forward<Args>(args)...));
	return m_operations.back().get();
}

class SDFPlane : public SDFObject
{
public:
	inline const static Vec3 DFLT_NORMAL{ Vec3::Up };
	inline const static double DFLT_OFFSET{ 0.0 };
private:
	Vec3 m_normal{};
	double m_offset{};
protected:
	std::string getClassName() const override { return "SDFPlane"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFPlane(Vec3 normal = DFLT_NORMAL, double offset = DFLT_OFFSET, Color color = Color::White)
		: SDFObject{ color }, m_normal(normal), m_offset(offset) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFBox : public SDFObject
{
public:
	inline const static Vec3 DFLT_DIMENSIONS{ 1.0 };
private:
	Vec3 m_dimensions;
protected:
	std::string getClassName() const override { return "SDFBox"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFBox(Vec3 dimensions = DFLT_DIMENSIONS, Color color = Color::White)
		: SDFObject(color), m_dimensions(dimensions) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFBoxFrame : public SDFObject
{
public:
	inline const static Vec3 DFLT_DIMENSIONS{ 1.0 };
	inline const static double DFLT_EDGETHICKNESS{ 0.1 };
private:
	Vec3 m_dimensions;
	double m_edgeThickness;
protected:
	std::string getClassName() const override { return "SDFBoxFrame"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFBoxFrame(Vec3 dimensions = DFLT_DIMENSIONS, double edgeThickness = DFLT_EDGETHICKNESS, Color color = Color::White)
		: SDFObject(color), m_dimensions(dimensions), m_edgeThickness(edgeThickness) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFSphere : public SDFObject
{
public:
	inline const static double DFLT_RADIUS{ 0.5 };
private:
	double m_radius;
protected:
	std::string getClassName() const override { return "SDFSphere"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFSphere(double radius = DFLT_RADIUS, Color color = Color::White)
		: SDFObject(color), m_radius(radius) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFTorusXY : public SDFObject
{
public:
	inline const static Vec2 DFLT_RADII{ 1.0, 0.25 };
private:
	Vec2 m_radii;
protected:
	std::string getClassName() const override { return "SDFTorusXY"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFTorusXY(Vec2 radii = DFLT_RADII, Color color = Color::White)
		: SDFObject(color), m_radii(radii) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFTorusXZ : public SDFObject
{
public:
	inline const static Vec2 DFLT_RADII{ 1.0, 0.25 };
private:
	Vec2 m_radii;
protected:
	std::string getClassName() const override { return "SDFTorusXZ"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFTorusXZ(Vec2 radii = DFLT_RADII, Color color = Color::White)
		: SDFObject(color), m_radii(radii) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFTorusYZ : public SDFObject
{
public:
	inline const static Vec2 DFLT_RADII{ 1.0, 0.25 };
private:
	Vec2 m_radii;
protected:
	std::string getClassName() const override { return "SDFTorusYZ"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFTorusYZ(Vec2 radii = DFLT_RADII, Color color = Color::White)
		: SDFObject(color), m_radii(radii) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFTorusSegmentXY : public SDFObject
{
public:
	inline const static Vec2 DFLT_RADII{ 1.0, 0.25 };
	inline const static double DFLT_SWEEP{ 0.25 };
private:
	double m_sweep;
	Vec2 m_radii;
	Vec2 m_sincos;
protected:
	std::string getClassName() const override { return "SDFTorusSegmentXY"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFTorusSegmentXY(Vec2 radii = DFLT_RADII, double sweep = DFLT_SWEEP, Color color = Color::White)
		: SDFObject(color), m_radii(radii), m_sweep(sweep), m_sincos(MF::fast_sincos(sweep* MF::PI)) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFTorusSegmentXZ : public SDFObject
{
public:
	inline const static Vec2 DFLT_RADII{ 1.0, 0.25 };
	inline const static double DFLT_SWEEP{ 0.25 };
private:
	Vec2 m_radii;
	double m_sweep;
	Vec2 m_sincos;
protected:
	std::string getClassName() const override { return "SDFTorusSegmentXZ"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFTorusSegmentXZ(Vec2 radii = DFLT_RADII, double sweep = DFLT_SWEEP, Color color = Color::White)
		: SDFObject(color), m_radii(radii), m_sweep(sweep), m_sincos(MF::fast_sincos(sweep* MF::PI)) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFTorusSegmentYZ : public SDFObject
{
public:
	inline const static Vec2 DFLT_RADII{ 1.0, 0.25 };
	inline const static double DFLT_SWEEP{ 0.25 };
private:
	Vec2 m_radii;
	double m_sweep;
	Vec2 m_sincos;
protected:
	std::string getClassName() const override { return "SDFTorusSegmentYZ"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFTorusSegmentYZ(Vec2 radii = DFLT_RADII, double sweep = DFLT_SWEEP, Color color = Color::White)
		: SDFObject(color), m_radii(radii), m_sweep(sweep), m_sincos(MF::fast_sincos(sweep* MF::PI)) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFCapsuleX : public SDFObject
{
public:
	inline const static double DFLT_RADIUS{ 0.25 };
	inline const static double DFLT_BODYLENGTH{ 1.0 };
private:
	double m_radius;
	double m_bodyLength;
protected:
	std::string getClassName() const override { return "SDFCapsuleX"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFCapsuleX(double radius = DFLT_RADIUS, double bodyLength = DFLT_BODYLENGTH, Color color = Color::White)
		: SDFObject(color), m_radius(radius), m_bodyLength(bodyLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFCapsuleY : public SDFObject
{
public:
	inline const static double DFLT_RADIUS{ 0.25 };
	inline const static double DFLT_BODYLENGTH{ 1.0 };
private:
	double m_radius;
	double m_bodyLength;
protected:
	std::string getClassName() const override { return "SDFCapsuleY"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFCapsuleY(double radius = DFLT_RADIUS, double bodyLength = DFLT_BODYLENGTH, Color color = Color::White)
		: SDFObject(color), m_radius(radius), m_bodyLength(bodyLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFCapsuleZ : public SDFObject
{
public:
	inline const static double DFLT_RADIUS{ 0.25 };
	inline const static double DFLT_BODYLENGTH{ 1.0 };
private:
	double m_radius;
	double m_bodyLength;
protected:
	std::string getClassName() const override { return "SDFCapsuleZ"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFCapsuleZ(double radius = DFLT_RADIUS, double bodyLength = DFLT_BODYLENGTH, Color color = Color::White)
		: SDFObject(color), m_radius(radius), m_bodyLength(bodyLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFCrystal : public SDFObject
{
public:
	inline const static double DFLT_EDGELENGTH{ 1.0 };
private:
	double m_edgeLength;
protected:
	std::string getClassName() const override { return "SDFCrystal"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFCrystal(double edgeLength = DFLT_EDGELENGTH, Color color = Color::White)
		: SDFObject(color), m_edgeLength(edgeLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFCylinderX : public SDFObject
{
public:
	inline const static double DFLT_RADIUS{ 0.5 };
	inline const static double DFLT_SIDELENGTH{ 1.0 };
private:
	double m_radius;
	double m_sideLength;
protected:
	std::string getClassName() const override { return "SDFCylinderX"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFCylinderX(double radius = DFLT_RADIUS, double sideLength = DFLT_SIDELENGTH, Color color = Color::White)
		: SDFObject(color), m_sideLength(sideLength), m_radius(radius) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFCylinderY : public SDFObject
{
public:
	inline const static double DFLT_RADIUS{ 0.5 };
	inline const static double DFLT_SIDELENGTH{ 1.0 };
private:
	double m_radius;
	double m_sideLength;
protected:
	std::string getClassName() const override { return "SDFCylinderY"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFCylinderY(double radius = DFLT_RADIUS, double sideLength = DFLT_SIDELENGTH, Color color = Color::White)
		: SDFObject(color), m_sideLength(sideLength), m_radius(radius) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFCylinderZ : public SDFObject
{
public:
	inline const static double DFLT_RADIUS{ 0.5 };
	inline const static double DFLT_SIDELENGTH{ 1.0 };
private:
	double m_radius;
	double m_sideLength;
protected:
	std::string getClassName() const override { return "SDFCylinderZ"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFCylinderZ(double radius = DFLT_RADIUS, double sideLength = DFLT_SIDELENGTH, Color color = Color::White)
		: SDFObject(color), m_sideLength(sideLength), m_radius(radius) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFCone : public SDFObject
{
public:
	inline const static double DFLT_HEIGHT{ 1.0 };
	inline const static double DFLT_SWEEP{ 0.5 };
private:
	double m_height{};
	double m_sweep{};
	Vec2 m_sincos{};
protected:
	std::string getClassName() const override { return "SDFCone"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFCone(double height = DFLT_HEIGHT, double sweep = DFLT_SWEEP, Color color = Color::White)
		: SDFObject(color), m_height(height), m_sweep(sweep), m_sincos(MF::fast_sincos(sweep * MF::HALFPI)) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFSolidAngle : public SDFObject
{
public:
	inline const static double DFLT_RADIUS{ 0.5 };
	inline const static double DFLT_SWEEP{ 0.25 };
private:
	double m_radius{};
	double m_sweep{};
	Vec2 m_sincos{};
protected:
	std::string getClassName() const override { return "SDFSolidAngle"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFSolidAngle(double radius = DFLT_RADIUS, double sweep = DFLT_SWEEP, Color color = Color::White)
		: SDFObject(color), m_radius(radius), m_sweep(sweep), m_sincos(MF::fast_sincos(sweep * MF::PI)) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFEllipsoid : public SDFObject
{
public:
	inline const static Vec3 DFLT_RADII{ 1.0, 0.5, 0.25 };
private:
	Vec3 m_radii;
protected:
	std::string getClassName() const override { return "SDFEllipsoid"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFEllipsoid(Vec3 radii = DFLT_RADII, Color color = Color::White)
		: SDFObject(color), m_radii(radii) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFTaperedCapsule : public SDFObject
{
public:
	inline const static Vec2 DFLT_RADII{ 0.5, 0.25 };
	inline const static double DFLT_BODYLENGTH{ 1.0 };
private:
	double m_bodyLength;
	Vec2 m_radii;
protected:
	std::string getClassName() const override { return "SDFTaperedCapsule"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFTaperedCapsule(Vec2 radii = DFLT_RADII, double bodyLength = DFLT_BODYLENGTH, Color color = Color::White)
		: SDFObject(color), m_radii(radii), m_bodyLength(bodyLength) {}
	double signedDistanceInner(Vec3 p) const override;
};

// Following SDFs are 2D shapes projected into 3D either with exrusion or revolution
// SDFs are adapted from https://iquilezles.org/articles/distfunctions2d/
// Extrusion/Revolution Ops are adapted from article linked near the top of this file
class SDFRhombus : public SDFObject
{
public:
	inline const static OP2D DFLT_OPERATION{ OP2D::Extrude };
	inline const static Vec2 DFLT_DIMENSIONS{ 1.0, 0.5 };
	inline const static double DFLT_MAGNITUDE{ 1.0 };
private:
	OP2D m_operation;
	Vec2 m_dimensions{};
	double m_magnitude;
protected:
	std::string getClassName() const override { return "SDFRhombus"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFRhombus(OP2D operation = DFLT_OPERATION, Vec2 dimensions = DFLT_DIMENSIONS, double magnitude = DFLT_MAGNITUDE, Color color = Color::White)
		: SDFObject(color), m_operation(operation), m_dimensions(dimensions), m_magnitude(magnitude) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFEquilateralTriangle : public SDFObject
{
public:
	inline const static OP2D DFLT_OPERATION{ OP2D::Extrude };
	inline const static double DFLT_BASE{ 1.0 };
	inline const static double DFLT_MAGNITUDE{ 1.0 };
private:
	OP2D m_operation;
	double m_base;
	double m_magnitude;
protected:
	std::string getClassName() const override { return "SDFEquilateralTriangle"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFEquilateralTriangle(OP2D operation = DFLT_OPERATION, double base = DFLT_BASE, double magnitude = DFLT_MAGNITUDE, Color color = Color::White)
		: SDFObject(color), m_operation(operation), m_base(base), m_magnitude(magnitude) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFIsoscelesTriangle : public SDFObject
{
public:
	inline const static OP2D DFLT_OPERATION{ OP2D::Extrude };
	inline const static Vec2 DFLT_DIMENSIONS{ 1.0, 0.5 };
	inline const static double DFLT_MAGNITUDE{ 1.0 };
private:
	OP2D m_operation;
	Vec2 m_dimensions;
	double m_magnitude;
protected:
	std::string getClassName() const override { return "SDFIsoscelesTriangle"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFIsoscelesTriangle(OP2D operation = DFLT_OPERATION, Vec2 dimensions = DFLT_DIMENSIONS, double magnitude = DFLT_MAGNITUDE, Color color = Color::White)
		: SDFObject(color), m_operation(operation), m_dimensions(dimensions), m_magnitude(magnitude) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFPieSlice : public SDFObject
{
public:
	inline const static OP2D DFLT_OPERATION{ OP2D::Extrude };
	inline const static double DFLT_SWEEP{ 0.25 };
	inline const static double DFLT_RADIUS{ 0.5 };
	inline const static double DFLT_MAGNITUDE{ 1.0 };
private:
	OP2D m_operation;
	double m_sweep;
	double m_radius;
	double m_magnitude;
	Vec2 m_sincos;
protected:
	std::string getClassName() const override { return "SDFPieSlice"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFPieSlice(OP2D operation = DFLT_OPERATION, double sweep = DFLT_SWEEP, double radius = DFLT_RADIUS, double magnitude = DFLT_MAGNITUDE, Color color = Color::White)
		: SDFObject(color), m_operation(operation), m_sweep(sweep), m_radius(radius), m_magnitude(magnitude), m_sincos(MF::fast_sincos(sweep * MF::PI)) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFPentagon : public SDFObject
{
public:
	inline const static OP2D DFLT_OPERATION{ OP2D::Extrude };
	inline const static double DFLT_RADIUS{ 0.5 };
	inline const static double DFLT_MAGNITUDE{ 1.0 };
private:
	OP2D m_operation;
	double m_radius;
	double m_magnitude;
protected:
	std::string getClassName() const override { return "SDFPentagon"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFPentagon(OP2D operation = DFLT_OPERATION, double radius = DFLT_RADIUS, double magnitude = DFLT_MAGNITUDE, Color color = Color::White)
		: SDFObject(color), m_operation(operation), m_radius(radius), m_magnitude(magnitude) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFHexagon : public SDFObject
{
public:
	inline const static OP2D DFLT_OPERATION{ OP2D::Extrude };
	inline const static double DFLT_RADIUS{ 0.5 };
	inline const static double DFLT_MAGNITUDE{ 1.0 };
private:
	OP2D m_operation;
	double m_radius;
	double m_magnitude;
protected:
	std::string getClassName() const override { return "SDFHexagon"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFHexagon(OP2D operation = DFLT_OPERATION, double radius = DFLT_RADIUS, double magnitude = DFLT_MAGNITUDE, Color color = Color::White)
		: SDFObject(color), m_operation(operation), m_radius(radius), m_magnitude(magnitude) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFOctagon : public SDFObject
{
public:
	inline const static OP2D DFLT_OPERATION{ OP2D::Extrude };
	inline const static double DFLT_RADIUS{ 0.5 };
	inline const static double DFLT_MAGNITUDE{ 1.0 };
private:
	OP2D m_operation;
	double m_radius;
	double m_magnitude;
protected:
	std::string getClassName() const override { return "SDFOctagon"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFOctagon(OP2D operation = DFLT_OPERATION, double radius = DFLT_RADIUS, double magnitude = DFLT_MAGNITUDE, Color color = Color::White)
		: SDFObject(color), m_operation(operation), m_radius(radius), m_magnitude(magnitude) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFStar : public SDFObject
{
public:
	inline const static OP2D DFLT_OPERATION{ OP2D::Extrude };
	inline const static size_t DFLT_NUMPOINT{ 5 };
	inline const static double DFLT_RADIUS{ 0.5 };
	inline const static double DFLT_RATIO{ 0.5 };
	inline const static double DFLT_MAGNITUDE{ 1.0 };
private:
	OP2D m_operation;
	size_t m_numPoints;
	double m_radius;
	double m_ratio;
	double m_magnitude;
	double m_an;
	double m_en;
	Vec2 m_asincos;
	Vec2 m_esincos;
protected:
	std::string getClassName() const override { return "SDFStar"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFStar(OP2D operation = DFLT_OPERATION, size_t numPoints = DFLT_NUMPOINT, double radius = DFLT_RADIUS, double ratio = DFLT_RATIO, double magnitude = DFLT_MAGNITUDE, Color color = Color::White)
		: SDFObject(color), m_operation(operation), m_numPoints(numPoints), m_radius(radius), m_ratio(ratio), m_magnitude(magnitude) , m_an(MF::PI / static_cast<double>(numPoints))
		,m_en(MF::PI / m_ratio), m_asincos(MF::fast_sincos(MF::PI / static_cast<double>(numPoints))), m_esincos(MF::fast_sincos(MF::PI / std::lerp(2.0, static_cast<double>(numPoints), ratio))) {}
	double signedDistanceInner(Vec3 p) const override;
};

class SDFTriangle : public SDFObject
{
public:
	inline const static OP2D DFLT_OPERATION{ OP2D::Extrude };
	inline const static Vec2 DFLT_POINT1{ -0.5, -0.5 };
	inline const static Vec2 DFLT_POINT2{ 0.0, 0.5 };
	inline const static Vec2 DFLT_POINT3{ 0.5, 0.0 };
	inline const static double DFLT_MAGNITUDE{ 1.0 };
private:
	OP2D m_operation;
	Vec2 m_point1;
	Vec2 m_point2;
	Vec2 m_point3;
	double m_magnitude;
protected:
	std::string getClassName() const override { return "SDFTriangle"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFTriangle(OP2D operation = DFLT_OPERATION, Vec2 point1 = DFLT_POINT1, Vec2 point2 = DFLT_POINT2, Vec2 point3 = DFLT_POINT3, double magnitude = DFLT_MAGNITUDE, Color color = Color::White)
		: SDFObject(color), m_operation(operation), m_point1(point1), m_point2(point2), m_point3(point3), m_magnitude(magnitude) {}
	double signedDistanceInner(Vec3 p) const override;
};

