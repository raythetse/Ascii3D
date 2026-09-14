#pragma once
#include "SDFOperation.h"
#include "SDFObject.h"

class CombinationOperation : public SDFOperation
{
protected:
	std::unique_ptr<SDFObject> m_combineObj{ nullptr };
	double m_smoothing;
	double m_colorBlending;
	double m_colorShifting;
	void blendColors(double blendRate, Vec3 colorIn, Vec3* colorOut);
	void innerUpdate() override {}
	virtual std::string getClassName() const = 0;
public:
	CombinationOperation(std::unique_ptr<SDFObject> combineObj, double smoothing = 0.5, double colorBlending = 1.0, double colorShifting = 0.0)
		: SDFOperation(SDFOperationType::Combine), m_combineObj(std::move(combineObj)), m_smoothing(smoothing), m_colorBlending(colorBlending), m_colorShifting(colorShifting) {}
	void update() override;
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
	void serialize(std::vector<std::string>& strVec, size_t level) const override;
	friend void SDFOperation::execute(Vec3& p, double& distance, const Vec3 colorIn, Vec3* colorOut);
};

class OPAdd : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
	std::string getClassName() const override { return "OPAdd"; }
public:
	OPAdd(std::unique_ptr<SDFObject> combineObj) : CombinationOperation(std::move(combineObj)) {}
};

class OPSubtract : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
	std::string getClassName() const override { return "OPSubtract"; }
public:
	OPSubtract(std::unique_ptr<SDFObject> combineObj) : CombinationOperation(std::move(combineObj)) {}
};

class OPIntersect : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
	std::string getClassName() const override { return "OPIntersect"; }
public:
	OPIntersect(std::unique_ptr<SDFObject> combineObj) : CombinationOperation(std::move(combineObj)) {}
};

class OPAddSmooth : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
	std::string getClassName() const override { return "OPAddSmooth"; }
public:
	OPAddSmooth(std::unique_ptr<SDFObject> combineObj, double smoothing = 0.5, double colorBlending = 1.0, double colorShifting = 0.0)
		: CombinationOperation(std::move(combineObj), smoothing, colorBlending, colorShifting) {}
};

class OPSubtractSmooth : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
	std::string getClassName() const override { return "OPSubtractSmooth"; }
public:
	OPSubtractSmooth(std::unique_ptr<SDFObject> objectPtr, double smoothing = 0.5, double colorBlending = 1.0, double colorShifting = 0.0)
		: CombinationOperation(std::move(objectPtr), smoothing, colorBlending, colorShifting) {}
};

class OPIntersectSmooth : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
	std::string getClassName() const override { return "OPIntersectSmooth"; }
public:
	OPIntersectSmooth(std::unique_ptr<SDFObject> objectPtr, double smoothing = 0.5, double colorBlending = 1.0, double colorShifting = 0.0)
		: CombinationOperation(std::move(objectPtr), smoothing, colorBlending, colorShifting) {}
};