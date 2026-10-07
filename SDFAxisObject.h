#pragma once
#include <string>
#include "Enumerations.h"
#include "SDFObject.h"
#include "Vec2.h"
#include "Vec3.h"

class SDFAxisObject : public SDFObject
{
protected:
	Vec3 getP(Vec3 p) const;
	std::string getPSwizzle() const;
	Axis m_axis;
	SDFAxisObject(Axis axis = AXIS_Y, Color color = Color::White) : SDFObject(color), m_axis(axis) {}
};

class SDFTorus : public SDFAxisObject
{
public:
	inline const static Vec2 DFLT_RADII{ 1.0, 0.25 };
private:
	Vec2 m_radii;
protected:
	std::string getClassName() const override { return "SDFTorus"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFTorus(Axis axis = AXIS_Y, Vec2 radii = DFLT_RADII, Color color = Color::White)
		: SDFAxisObject(axis, color), m_radii(radii) {}
	double signedDistanceInner(Vec3 p) const override;
	virtual void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

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
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFTorusSegment(Axis axis = AXIS_Y, Vec2 radii = DFLT_RADII, double sweep = DFLT_SWEEP, Color color = Color::White)
		: SDFAxisObject(axis, color), m_radii(radii), m_sweep(sweep) {}
	double signedDistanceInner(Vec3 p) const override;
	virtual void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};