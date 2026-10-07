#pragma once
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include "Enumerations.h"
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
protected:
	Color m_color{};
	std::vector<std::unique_ptr<SDFOperation>> m_operations;
	std::vector<std::unique_ptr<PropertyModifierBase>> m_propMods{};
	virtual double signedDistanceInner(Vec3 p) const;
	virtual void serializeInner(std::stringstream& ss) const {};
	virtual std::string getClassName() const { return "SDFObject"; }
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const {};
	double extrude2D(double pDotY, double d2D, double magnitude) const;
	Vec3 revolve2D(Vec3 p, double radius) const;
	bool getShaderPropModCalls(std::string& propName) const;
public:
	SDFObject(Color color = Color::White)
		: m_color{ color } {}
	~SDFObject();
	void update();
	virtual double signedDistance(Vec3 p, Color* colorOut = nullptr) const;
	SDFOperation* pushOperation(std::unique_ptr<SDFOperation> operation);
	template <typename T, typename... Args>
	SDFOperation* pushOperation(Args&&... args);
	void setColor(Color color);
	Color getColor();
	void resetMods();
	virtual void serialize(std::vector<std::string> &strVec, size_t level) const;
	void getShaderCalls(std::stringstream& ss, size_t level, size_t& maxLevel);
	virtual void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod);
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
	virtual void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
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
	virtual void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
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
	virtual void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
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
	virtual void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
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
	virtual void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class SDFCone : public SDFObject
{
public:
	inline const static double DFLT_HEIGHT{ 1.0 };
	inline const static double DFLT_SWEEP{ 0.5 };
private:
	double m_height{};
	double m_sweep{};
protected:
	std::string getClassName() const override { return "SDFCone"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFCone(double height = DFLT_HEIGHT, double sweep = DFLT_SWEEP, Color color = Color::White)
		: SDFObject(color), m_height(height), m_sweep(sweep) {}
	double signedDistanceInner(Vec3 p) const override;
	virtual void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class SDFSolidAngle : public SDFObject
{
public:
	inline const static double DFLT_RADIUS{ 0.5 };
	inline const static double DFLT_SWEEP{ 0.25 };
private:
	double m_radius{};
	double m_sweep{};
protected:
	std::string getClassName() const override { return "SDFSolidAngle"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderObjectCalls(std::stringstream& ss, size_t level, size_t highestLevel) const override;
public:
	SDFSolidAngle(double radius = DFLT_RADIUS, double sweep = DFLT_SWEEP, Color color = Color::White)
		: SDFObject(color), m_radius(radius), m_sweep(sweep) {}
	double signedDistanceInner(Vec3 p) const override;
	virtual void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
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
	virtual void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};
