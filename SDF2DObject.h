#pragma once
#include "SDFObject.h"

// The following SDFs are 2D shapes projected into 3D space with infite extrusion, limited extrusion, or revolution
// SDFs are adapted from https://iquilezles.org/articles/distfunctions2d/
// Extrusion/Revolution operations are adapted from https://iquilezles.org/articles/distfunctions/

class SDF2DObject : public SDFObject
{
public:
	inline const static OP2D DFLT_OPERATION{ OP2D::OP2D_EXTRUDE };
	inline const static double DFLT_MAGNITUDE{ 1.0 };
	inline const static double DFLT_REVOLVEROTATIONS{ 0.0 };
	inline const static Vec2 DFLT_REVOLVEROTATEOFFSET{ 0.0 };
protected:
	OP2D m_operation;
	double m_magnitude;
	double m_revolveRotations;
	Vec2 m_revolveRotateOffset;
	SDF2DObject(OP2D operation, double magnitude = DFLT_MAGNITUDE, double revolveRotations = DFLT_REVOLVEROTATIONS, Vec2 revolveRotateOffset = DFLT_REVOLVEROTATEOFFSET, Color color = Color::White)
		: SDFObject(color), m_operation(operation), m_magnitude(magnitude), m_revolveRotations(revolveRotations), m_revolveRotateOffset(revolveRotateOffset) {}
	virtual double signedDistanceInnerInner(Vec3 p) const = 0;
	virtual void serializeInnerInner(std::stringstream& ss) const = 0;
	virtual void bindPropertyModifierInner(std::unique_ptr<PropertyModifierBase> propMod) = 0;
public:
	void serializeInner(std::stringstream& ss) const override;
	double signedDistanceInner(Vec3 p) const override;
	virtual void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class SDFRhombus : public SDF2DObject
{
public:
	inline const static Vec2 DFLT_DIMENSIONS{ 1.0, 0.5 };
private:
	Vec2 m_dimensions{};
protected:
	std::string getClassName() const override { return "SDFRhombus"; }
	void serializeInnerInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFRhombus(OP2D operation = DFLT_OPERATION, Vec2 dimensions = DFLT_DIMENSIONS, double magnitude = DFLT_MAGNITUDE
		, double revolveRotations = DFLT_REVOLVEROTATIONS, Vec2 revolveRotateOffset = DFLT_REVOLVEROTATEOFFSET, Color color = Color::White)
		: SDF2DObject(operation, magnitude, revolveRotations, revolveRotateOffset, color), m_dimensions(dimensions) {}
	double signedDistanceInnerInner(Vec3 p) const override;
	virtual void bindPropertyModifierInner(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class SDFEquilateralTriangle : public SDF2DObject
{
public:
	inline const static OP2D DFLT_OPERATION{ OP2D::OP2D_EXTRUDE };
	inline const static double DFLT_BASE{ 1.0 };
	inline const static double DFLT_MAGNITUDE{ 1.0 };
private:
	double m_base;
protected:
	std::string getClassName() const override { return "SDFEquilateralTriangle"; }
	void serializeInnerInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFEquilateralTriangle(OP2D operation = DFLT_OPERATION, double base = DFLT_BASE, double magnitude = DFLT_MAGNITUDE
		, double revolveRotations = DFLT_REVOLVEROTATIONS, Vec2 revolveRotateOffset = DFLT_REVOLVEROTATEOFFSET, Color color = Color::White)
		: SDF2DObject(operation, magnitude, revolveRotations, revolveRotateOffset, color), m_base(base) {}
	double signedDistanceInnerInner(Vec3 p) const override;
	virtual void bindPropertyModifierInner(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class SDFIsoscelesTriangle : public SDF2DObject
{
public:
	inline const static OP2D DFLT_OPERATION{ OP2D::OP2D_EXTRUDE };
	inline const static Vec2 DFLT_DIMENSIONS{ 1.0, 0.5 };
	inline const static double DFLT_MAGNITUDE{ 1.0 };
private:
	Vec2 m_dimensions;
protected:
	std::string getClassName() const override { return "SDFIsoscelesTriangle"; }
	void serializeInnerInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFIsoscelesTriangle(OP2D operation = DFLT_OPERATION, Vec2 dimensions = DFLT_DIMENSIONS, double magnitude = DFLT_MAGNITUDE
		, double revolveRotations = DFLT_REVOLVEROTATIONS, Vec2 revolveRotateOffset = DFLT_REVOLVEROTATEOFFSET, Color color = Color::White)
		: SDF2DObject(operation, magnitude, revolveRotations, revolveRotateOffset, color), m_dimensions(dimensions) {}
	double signedDistanceInnerInner(Vec3 p) const override;
	virtual void bindPropertyModifierInner(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class SDFTriangle : public SDF2DObject
{
public:
	inline const static OP2D DFLT_OPERATION{ OP2D::OP2D_EXTRUDE };
	inline const static Vec2 DFLT_POINT1{ -0.5, -0.5 };
	inline const static Vec2 DFLT_POINT2{ 0.0, 0.5 };
	inline const static Vec2 DFLT_POINT3{ 0.5, 0.0 };
	inline const static double DFLT_MAGNITUDE{ 1.0 };
private:
	Vec2 m_point1;
	Vec2 m_point2;
	Vec2 m_point3;
protected:
	std::string getClassName() const override { return "SDFTriangle"; }
	void serializeInnerInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFTriangle(OP2D operation = DFLT_OPERATION, Vec2 point1 = DFLT_POINT1, Vec2 point2 = DFLT_POINT2, Vec2 point3 = DFLT_POINT3, double magnitude = DFLT_MAGNITUDE
		, double revolveRotations = DFLT_REVOLVEROTATIONS, Vec2 revolveRotateOffset = DFLT_REVOLVEROTATEOFFSET, Color color = Color::White)
		: SDF2DObject(operation, magnitude, revolveRotations, revolveRotateOffset, color), m_point1(point1), m_point2(point2), m_point3(point3) {}
	double signedDistanceInnerInner(Vec3 p) const override;
	virtual void bindPropertyModifierInner(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class SDFPieSlice : public SDF2DObject
{
public:
	inline const static double DFLT_SWEEP{ 0.25 };
	inline const static double DFLT_RADIUS{ 0.5 };
private:
	double m_sweep;
	double m_radius;
protected:
	std::string getClassName() const override { return "SDFPieSlice"; }
	void serializeInnerInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFPieSlice(OP2D operation = DFLT_OPERATION, double sweep = DFLT_SWEEP, double radius = DFLT_RADIUS, double magnitude = DFLT_MAGNITUDE
		, double revolveRotations = DFLT_REVOLVEROTATIONS, Vec2 revolveRotateOffset = DFLT_REVOLVEROTATEOFFSET, Color color = Color::White)
		: SDF2DObject(operation, magnitude, revolveRotations, revolveRotateOffset, color), m_sweep(sweep), m_radius(radius) {}
	double signedDistanceInnerInner(Vec3 p) const override;
	virtual void bindPropertyModifierInner(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class SDFPentagon : public SDF2DObject
{
public:
	inline const static OP2D DFLT_OPERATION{ OP2D::OP2D_EXTRUDE };
	inline const static double DFLT_RADIUS{ 0.5 };
	inline const static double DFLT_MAGNITUDE{ 1.0 };
private:
	double m_radius;
protected:
	std::string getClassName() const override { return "SDFPentagon"; }
	void serializeInnerInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFPentagon(OP2D operation = DFLT_OPERATION, double radius = DFLT_RADIUS, double magnitude = DFLT_MAGNITUDE
		, double revolveRotations = DFLT_REVOLVEROTATIONS, Vec2 revolveRotateOffset = DFLT_REVOLVEROTATEOFFSET, Color color = Color::White)
		: SDF2DObject(operation, magnitude, revolveRotations, revolveRotateOffset, color), m_radius(radius) {}
	double signedDistanceInnerInner(Vec3 p) const override;
	virtual void bindPropertyModifierInner(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class SDFHexagon : public SDF2DObject
{
public:
	inline const static OP2D DFLT_OPERATION{ OP2D::OP2D_EXTRUDE };
	inline const static double DFLT_RADIUS{ 0.5 };
	inline const static double DFLT_MAGNITUDE{ 1.0 };
private:
	double m_radius;
protected:
	std::string getClassName() const override { return "SDFHexagon"; }
	void serializeInnerInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFHexagon(OP2D operation = DFLT_OPERATION, double radius = DFLT_RADIUS, double magnitude = DFLT_MAGNITUDE
		, double revolveRotations = DFLT_REVOLVEROTATIONS, Vec2 revolveRotateOffset = DFLT_REVOLVEROTATEOFFSET, Color color = Color::White)
		: SDF2DObject(operation, magnitude, revolveRotations, revolveRotateOffset, color), m_radius(radius) {}
	double signedDistanceInnerInner(Vec3 p) const override;
	virtual void bindPropertyModifierInner(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class SDFOctagon : public SDF2DObject
{
public:
	inline const static OP2D DFLT_OPERATION{ OP2D::OP2D_EXTRUDE };
	inline const static double DFLT_RADIUS{ 0.5 };
	inline const static double DFLT_MAGNITUDE{ 1.0 };
private:
	double m_radius;
protected:
	std::string getClassName() const override { return "SDFOctagon"; }
	void serializeInnerInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFOctagon(OP2D operation = DFLT_OPERATION, double radius = DFLT_RADIUS, double magnitude = DFLT_MAGNITUDE
		, double revolveRotations = DFLT_REVOLVEROTATIONS, Vec2 revolveRotateOffset = DFLT_REVOLVEROTATEOFFSET, Color color = Color::White)
		: SDF2DObject(operation, magnitude, revolveRotations, revolveRotateOffset, color), m_radius(radius) {}
	double signedDistanceInnerInner(Vec3 p) const override;
	virtual void bindPropertyModifierInner(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class SDFStar : public SDF2DObject
{
public:
	inline const static OP2D DFLT_OPERATION{ OP2D::OP2D_EXTRUDE };
	inline const static size_t DFLT_NUMPOINT{ 5 };
	inline const static double DFLT_RADIUS{ 0.5 };
	inline const static double DFLT_RATIO{ 0.5 };
	inline const static double DFLT_MAGNITUDE{ 1.0 };
private:
	size_t m_numPoints;
	double m_radius;
	double m_ratio;
protected:
	std::string getClassName() const override { return "SDFStar"; }
	void serializeInnerInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFStar(OP2D operation = DFLT_OPERATION, size_t numPoints = DFLT_NUMPOINT, double radius = DFLT_RADIUS, double ratio = DFLT_RATIO, double magnitude = DFLT_MAGNITUDE
		, double revolveRotations = DFLT_REVOLVEROTATIONS, Vec2 revolveRotateOffset = DFLT_REVOLVEROTATEOFFSET, Color color = Color::White)
		: SDF2DObject(operation, magnitude, revolveRotations, revolveRotateOffset, color), m_numPoints(numPoints), m_radius(radius), m_ratio(ratio) {}
	double signedDistanceInnerInner(Vec3 p) const override;
	virtual void bindPropertyModifierInner(std::unique_ptr<PropertyModifierBase> propMod) override;
};