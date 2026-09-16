#pragma once
#include "SDFOperation.h"
#include "SDFObject.h"
#include "SDFDefaults.h"

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
	void innerSerialize(std::vector<std::string>& strVec, size_t level) const override;
public:
	CombinationOperation(std::unique_ptr<SDFObject> combineObj,
		double smoothing = DEF::COMBSmooth,
		double colorBlending = DEF::COMBColBlend,
		double colorShifting = DEF::COMBColShift)
		: SDFOperation(SDFOperationType::Combine), m_combineObj(std::move(combineObj)), m_smoothing(smoothing), m_colorBlending(colorBlending), m_colorShifting(colorShifting) {}
	void setCombineObject(std::unique_ptr<SDFObject> combineObj);
	void update() override;
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
	void serialize(std::vector<std::string>& strVec, size_t level) const override;
	friend void SDFOperation::execute(Vec3& p, double& distance, const Vec3 colorIn, Vec3* colorOut);
};

class COMBAdd : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
	std::string getClassName() const override { return "COMBAdd"; }
public:
	COMBAdd(std::unique_ptr<SDFObject> combineObj = nullptr) : CombinationOperation(std::move(combineObj)) {}
};

class COMBSubtract : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
	std::string getClassName() const override { return "COMBSubtract"; }
public:
	COMBSubtract(std::unique_ptr<SDFObject> combineObj = nullptr) : CombinationOperation(std::move(combineObj)) {}
};

class COMBIntersect : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
	std::string getClassName() const override { return "COMBIntersect"; }
public:
	COMBIntersect(std::unique_ptr<SDFObject> combineObj = nullptr) : CombinationOperation(std::move(combineObj)) {}
};

class COMBAddSmooth : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
	std::string getClassName() const override { return "COMBAddSmooth"; }
public:
	COMBAddSmooth(std::unique_ptr<SDFObject> combineObj = nullptr,
		double smoothing = DEF::COMBSmooth,
		double colorBlending = DEF::COMBColBlend,
		double colorShifting = DEF::COMBColShift)
		: CombinationOperation(std::move(combineObj), smoothing, colorBlending, colorShifting) {}
};

class COMBSubtractSmooth : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
	std::string getClassName() const override { return "COMBSubtractSmooth"; }
public:
	COMBSubtractSmooth(std::unique_ptr<SDFObject> objectPtr = nullptr,
		double smoothing = DEF::COMBSmooth,
		double colorBlending = DEF::COMBColBlend,
		double colorShifting = DEF::COMBColShift)
		: CombinationOperation(std::move(objectPtr), smoothing, colorBlending, colorShifting) {}
};

class COMBIntersectSmooth : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
	std::string getClassName() const override { return "COMBIntersectSmooth"; }
public:
	COMBIntersectSmooth(std::unique_ptr<SDFObject> objectPtr = nullptr,
		double smoothing = DEF::COMBSmooth,
		double colorBlending = DEF::COMBColBlend,
		double colorShifting = DEF::COMBColShift)
		: CombinationOperation(std::move(objectPtr), smoothing, colorBlending, colorShifting) {}
};