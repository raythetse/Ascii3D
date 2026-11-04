#pragma once
#include <string>
#include "Enumerations.h"
#include "SDFObject.h"
#include "Vec2.h"
#include "Vec3.h"

// Axis objects can be initially alligned with the x, y, or z axis.
class SDFAxisObject : public SDFObject
{
protected:
	Vec3 getP(Vec3 p) const;
	std::string getPSwizzle() const;
	Axis m_axis;
	SDFAxisObject(Axis axis = AXIS_Y, Color color = Color::White) : SDFObject(color), m_axis(axis) {}
};

// Standard torus defined by its major and minor radii
class SDFTorus : public SDFAxisObject
{
public:
	inline const static Vec2 DFLT_RADII{ 1.0, 0.25 };
private:
	Vec2 m_radii;
protected:
	std::string getClassName() const override { return "SDFTorus"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level) const override;
public:
	SDFTorus(Axis axis = AXIS_Y, Vec2 radii = DFLT_RADII, Color color = Color::White)
		: SDFAxisObject(axis, color), m_radii(radii) {}
	double signedDistanceInner(Vec3 p) const override;
	virtual BindResult bindAnimation(std::unique_ptr<Animation> animation) override;
};

// A portion of a torus defined by its major and minor radii and the angle (0.0 - 1.0) that it sweeps out
class SDFTorusSegment : public SDFAxisObject
{
public:
	inline const static Vec2 DFLT_RADII{ 1.0, 0.25 };
	inline const static double DFLT_SWEEP{ 0.25 };
private:
	double m_sweep;
	Vec2 m_radii;
protected:
	std::string getClassName() const override { return "SDFTorusSegment"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level) const override;
public:
	SDFTorusSegment(Axis axis = AXIS_Y, Vec2 radii = DFLT_RADII, double sweep = DFLT_SWEEP, Color color = Color::White)
		: SDFAxisObject(axis, color), m_radii(radii), m_sweep(sweep) {}
	double signedDistanceInner(Vec3 p) const override;
	virtual BindResult bindAnimation(std::unique_ptr<Animation> animation) override;
};

// A cylinder with rounded off ends defined by its radius and body length
class SDFCapsule : public SDFAxisObject
{
public:
	inline const static double DFLT_RADIUS{ 0.5 };
	inline const static double DFLT_BODYLENGTH{ 1.0 };
private:
	double m_radius;
	double m_bodyLength;
protected:
	std::string getClassName() const override { return "SDFCapsule"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level) const override;
public:
	SDFCapsule(Axis axis = AXIS_Y, double radius = DFLT_RADIUS, double bodyLength = DFLT_BODYLENGTH, Color color = Color::White)
		: SDFAxisObject(axis, color), m_radius(radius), m_bodyLength(bodyLength) {}
	double signedDistanceInner(Vec3 p) const override;
	virtual BindResult bindAnimation(std::unique_ptr<Animation> animation) override;
};

// A rounded-off cylinder with a tapering radius
class SDFTaperedCapsule : public SDFAxisObject
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
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level) const override;
public:
	SDFTaperedCapsule(Axis axis = AXIS_Y, Vec2 radii = DFLT_RADII, double bodyLength = DFLT_BODYLENGTH, Color color = Color::White)
		: SDFAxisObject(axis, color), m_radii(radii), m_bodyLength(bodyLength) {}
	double signedDistanceInner(Vec3 p) const override;
	virtual BindResult bindAnimation(std::unique_ptr<Animation> animation) override;
};

// A non-rounded cylinder
class SDFCylinder : public SDFAxisObject
{
public:
	inline const static double DFLT_RADIUS{ 0.5 };
	inline const static double DFLT_SIDELENGTH{ 1.0 };
private:
	double m_radius;
	double m_sideLength;
protected:
	std::string getClassName() const override { return "SDFCylinder"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level) const override;
public:
	SDFCylinder(Axis axis = Axis::AXIS_Y, double radius = DFLT_RADIUS, double sideLength = DFLT_SIDELENGTH, Color color = Color::White)
		: SDFAxisObject(axis, color), m_sideLength(sideLength), m_radius(radius) {}
	double signedDistanceInner(Vec3 p) const override;
	virtual BindResult bindAnimation(std::unique_ptr<Animation> animation) override;
};

// A non-rounded tapered cylinder
class SDFTaperedCylinder : public SDFAxisObject
{
public:
	inline const static Vec2 DFLT_RADII{ 0.5, 0.25 };
	inline const static double DFLT_SIDELENGTH{ 1.0 };
private:
	Vec2 m_radii;
	double m_sideLength;
protected:
	std::string getClassName() const override { return "SDFTaperedCylinder"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level) const override;
public:
	SDFTaperedCylinder(Axis axis = Axis::AXIS_Y, Vec2 radii = DFLT_RADII, double sideLength = DFLT_SIDELENGTH, Color color = Color::White)
		: SDFAxisObject(axis, color), m_sideLength(sideLength), m_radii(radii) {}
	double signedDistanceInner(Vec3 p) const override;
	virtual BindResult bindAnimation(std::unique_ptr<Animation> animation) override;
};