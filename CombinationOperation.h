#pragma once
#include "SDFOperation.h"
#include "SDFObject.h"

class CombinationOperation : public SDFOperation
{
protected:
	std::unique_ptr<SDFObject> m_combineObj{ nullptr };
	double m_smoothing;
	double m_colorBlending;
	void blendColors(double blendRate, Vec3 colorIn, Vec3* colorOut);
	void innerUpdate() override {}
public:
	CombinationOperation(std::unique_ptr<SDFObject> combineObj, double smoothing = 0.5, double colorBlending = 1.0)
		: m_combineObj(std::move(combineObj)), m_smoothing(smoothing), m_colorBlending(colorBlending) {}
	void update() override;
	SDFOperationType opType() override { return SDFOperationType::Combine; }
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
	friend void SDFOperation::execute(Vec3& p, double& distance, const Vec3 colorIn, Vec3* colorOut);
};

class Add : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
public:
	Add(std::unique_ptr<SDFObject> combineObj) : CombinationOperation(std::move(combineObj)) {}
};

class Subtract : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
public:
	Subtract(std::unique_ptr<SDFObject> combineObj) : CombinationOperation(std::move(combineObj)) {}
};

class Intersect : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
public:
	Intersect(std::unique_ptr<SDFObject> combineObj) : CombinationOperation(std::move(combineObj)) {}
};

class AddSmooth : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
public:
	AddSmooth(std::unique_ptr<SDFObject> combineObj, double smoothing = 0.5, double colorBlending = 1.0)
		: CombinationOperation(std::move(combineObj), smoothing, colorBlending) {}
};

class SubtractSmooth : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
public:
	SubtractSmooth(std::unique_ptr<SDFObject> objectPtr, double smoothing = 0.5, double colorBlending = 1.0)
		: CombinationOperation(std::move(objectPtr), smoothing, colorBlending) {}
};

class IntersectSmooth : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
public:
	IntersectSmooth(std::unique_ptr<SDFObject> objectPtr, double smoothing = 0.5, double colorBlending = 1.0)
		: CombinationOperation(std::move(objectPtr), smoothing, colorBlending) {}
};