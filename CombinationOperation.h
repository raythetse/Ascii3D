#pragma once
#include "Animation.h"
#include "SDFOperation.h"
#include "SDFObjectInc.h"

// Combinations allow two objects or object trees to be merged together in various ways.
class CombinationOperation : public SDFOperation
{
private:
	double m_onTime{ 1.0 };
	double m_offTime{ 0.0 };
	double m_phase{ 0.0 };
	bool m_isOn{ true };
protected:
	bool m_overwriteColor{};
	std::unique_ptr<SDFObject> m_combineObj{ nullptr };
	void innerUpdate() override {}
	virtual std::string getClassName() const = 0;
	void innerSerialize(std::vector<std::string>& strVec, size_t level) const override;
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const {};
	virtual void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const;
	virtual void combine(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const = 0;
	CombinationOperation(std::unique_ptr<SDFObject> combineObj, double onTime = 1.0, double offTime = 0.0, double phase = 0.0, bool overwriteColor = false)
		: m_combineObj(std::move(combineObj)), m_onTime(onTime), m_offTime(offTime), m_phase(phase), m_overwriteColor(overwriteColor) {}
public:
	virtual BindResult bindAnimation(std::unique_ptr<Animation> animation) override { return BindResult::UnknownProperty; }
	void setCombineObject(std::unique_ptr<SDFObject> combineObj);
	virtual void update() override;
	virtual void resetAnimations() override;
	void serialize(std::vector<std::string>& strVec, size_t level) const override;
	void getShaderCalls(std::stringstream& ss, size_t level) const override;
	virtual ~CombinationOperation() = default;
};

// Simple adding of objects together
class COMBAdd : public CombinationOperation
{
protected:
	virtual void combine(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const override;
	std::string getClassName() const override { return "COMBAdd"; }
	void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	COMBAdd(std::unique_ptr<SDFObject> combineObj = nullptr, bool overwriteColor = false, double onTime = 1.0, double offTime = 0.0, double phase = 0.0)
		: CombinationOperation(std::move(combineObj), onTime, offTime, phase, overwriteColor) {}
};

// Subtracting and object/group from the scene (cutting shaped holes in objects)
class COMBSubtract : public CombinationOperation
{
protected:
	virtual void combine(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const override;
	std::string getClassName() const override { return "COMBSubtract"; }
	void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	COMBSubtract(std::unique_ptr<SDFObject> combineObj = nullptr) : CombinationOperation(std::move(combineObj)) {}
};

// Keeping only overlapped spaces between two objects/groups
class COMBIntersect : public CombinationOperation
{
protected:
	virtual void combine(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const override;
	std::string getClassName() const override { return "COMBIntersect"; }
	void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	COMBIntersect(std::unique_ptr<SDFObject> combineObj = nullptr) : CombinationOperation(std::move(combineObj)) {}
};

// Keeping only non-overlapped spaces betwen two objects/groups
class COMBExclude : public CombinationOperation
{
private:
	ColorMode m_colorMode;
protected:
	virtual void combine(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const override;
	std::string getClassName() const override { return "COMBExclude"; }
	void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
	void innerSerialize(std::vector<std::string>& strVec, size_t level) const override;
public:
	COMBExclude(std::unique_ptr<SDFObject> combineObj = nullptr, ColorMode colorMode = COLORMODE_DOUBLE) : CombinationOperation(std::move(combineObj)), m_colorMode(colorMode) {}
};

// Smooth combinations perform the same operations as the previous combinations but with smoothy rounded edges/corners
class SmoothCombination : public CombinationOperation
{
public:
	inline const static double DFLT_SMOOTHING{ 0.5 };
	inline const static double DFLT_COLORBLENDING{ 1.0 };
	inline const static double DFLT_COLORSHIFTING{ 0.0 };
protected:
	double m_smoothing;
	double m_colorBlending;
	double m_colorShifting;
	void blendColors(double blendRate, Vec3 colorIn, Vec3* colorOut) const;
	BindResult bindAnimation(std::unique_ptr<Animation> animation) override;
	void innerSerialize(std::vector<std::string>& strVec, size_t level) const override;
public:
	SmoothCombination(std::unique_ptr<SDFObject> combineObj, double smoothing = DFLT_SMOOTHING, double colorBlending = DFLT_COLORBLENDING, double colorShifting = DFLT_COLORSHIFTING)
		: CombinationOperation(std::move(combineObj)), m_smoothing(smoothing), m_colorBlending(colorBlending), m_colorShifting(colorShifting) {}
};

class COMBAddSmooth : public SmoothCombination
{
protected:
	virtual void combine(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const override;
	std::string getClassName() const override { return "COMBAddSmooth"; }
	void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	COMBAddSmooth(std::unique_ptr<SDFObject> combineObj = nullptr, double smoothing = DFLT_SMOOTHING, double colorBlending = DFLT_COLORBLENDING, double colorShifting = DFLT_COLORSHIFTING)
		: SmoothCombination(std::move(combineObj), smoothing, colorBlending, colorShifting) {}
};

class COMBSubtractSmooth : public SmoothCombination
{
protected:
	virtual void combine(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const override;
	std::string getClassName() const override { return "COMBSubtractSmooth"; }
	void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	COMBSubtractSmooth(std::unique_ptr<SDFObject> objectPtr = nullptr, double smoothing = DFLT_SMOOTHING, double colorBlending = DFLT_COLORBLENDING, double colorShifting = DFLT_COLORSHIFTING)
		: SmoothCombination(std::move(objectPtr), smoothing, colorBlending, colorShifting) {}
};

class COMBIntersectSmooth : public SmoothCombination
{
protected:
	virtual void combine(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const override;
	std::string getClassName() const override { return "COMBIntersectSmooth"; }
	void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	COMBIntersectSmooth(std::unique_ptr<SDFObject> objectPtr = nullptr,	double smoothing = DFLT_SMOOTHING, double colorBlending = DFLT_COLORBLENDING, double colorShifting = DFLT_COLORSHIFTING)
		: SmoothCombination(std::move(objectPtr), smoothing, colorBlending, colorShifting) {}
};

class COMBExcludeSmooth : public SmoothCombination
{
protected:
	virtual void combine(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const override;
	std::string getClassName() const override { return "COMBExcludeSmooth"; }
	void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	COMBExcludeSmooth(std::unique_ptr<SDFObject> objectPtr = nullptr, double smoothing = DFLT_SMOOTHING, double colorBlending = DFLT_COLORBLENDING, double colorShifting = DFLT_COLORSHIFTING)
		: SmoothCombination(std::move(objectPtr), smoothing, colorBlending, colorShifting) {}
};