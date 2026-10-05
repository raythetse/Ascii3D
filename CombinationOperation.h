#pragma once
#include "SDFOperation.h"
#include "SDFObject.h"

class CombinationOperation : public SDFOperation
{
protected:
	std::unique_ptr<SDFObject> m_combineObj{ nullptr };
	void innerUpdate() override {}
	virtual std::string getClassName() const = 0;
	void innerSerialize(std::vector<std::string>& strVec, size_t level) const override;
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const {};
	virtual void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override {};
public:
	CombinationOperation(std::unique_ptr<SDFObject> combineObj)
		: SDFOperation(SDFOperationType::Combine), m_combineObj(std::move(combineObj)) {}
	void setCombineObject(std::unique_ptr<SDFObject> combineObj);
	void update() override;
	void serialize(std::vector<std::string>& strVec, size_t level) const override;
	void getShaderCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
	friend void SDFOperation::execute(Vec3& p, double& distance, const Vec3 colorIn, Vec3* colorOut);
};

class COMBAdd : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
	std::string getClassName() const override { return "COMBAdd"; }
	void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	COMBAdd(std::unique_ptr<SDFObject> combineObj = nullptr) : CombinationOperation(std::move(combineObj)) {}
};

class COMBSubtract : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
	std::string getClassName() const override { return "COMBSubtract"; }
	void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	COMBSubtract(std::unique_ptr<SDFObject> combineObj = nullptr) : CombinationOperation(std::move(combineObj)) {}
};

class COMBIntersect : public CombinationOperation
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
	std::string getClassName() const override { return "COMBIntersect"; }
	void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	COMBIntersect(std::unique_ptr<SDFObject> combineObj = nullptr) : CombinationOperation(std::move(combineObj)) {}
};

class COMBExclude : public CombinationOperation
{
public:
	enum ColorMode {
		Double,
		Single,
		Internal,
		InternalDouble
	};
private:
	ColorMode m_colorMode;
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
	std::string getClassName() const override { return "COMBExclude"; }
	void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
	void innerSerialize(std::vector<std::string>& strVec, size_t level) const override;
public:
	COMBExclude(std::unique_ptr<SDFObject> combineObj = nullptr, ColorMode colorMode = Double) : CombinationOperation(std::move(combineObj)), m_colorMode(colorMode) {}
};

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
	void blendColors(double blendRate, Vec3 colorIn, Vec3* colorOut);
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
	void innerSerialize(std::vector<std::string>& strVec, size_t level) const override;
public:
	SmoothCombination(std::unique_ptr<SDFObject> combineObj, double smoothing = DFLT_SMOOTHING, double colorBlending = DFLT_COLORBLENDING, double colorShifting = DFLT_COLORSHIFTING)
		: CombinationOperation(std::move(combineObj)), m_smoothing(smoothing), m_colorBlending(colorBlending), m_colorShifting(colorShifting) {}
};

class COMBAddSmooth : public SmoothCombination
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
	std::string getClassName() const override { return "COMBAddSmooth"; }
	void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	COMBAddSmooth(std::unique_ptr<SDFObject> combineObj = nullptr, double smoothing = DFLT_SMOOTHING, double colorBlending = DFLT_COLORBLENDING, double colorShifting = DFLT_COLORSHIFTING)
		: SmoothCombination(std::move(combineObj), smoothing, colorBlending, colorShifting) {}
};

class COMBSubtractSmooth : public SmoothCombination
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
	std::string getClassName() const override { return "COMBSubtractSmooth"; }
	void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	COMBSubtractSmooth(std::unique_ptr<SDFObject> objectPtr = nullptr, double smoothing = DFLT_SMOOTHING, double colorBlending = DFLT_COLORBLENDING, double colorShifting = DFLT_COLORSHIFTING)
		: SmoothCombination(std::move(objectPtr), smoothing, colorBlending, colorShifting) {}
};

class COMBIntersectSmooth : public SmoothCombination
{
protected:
	double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) override;
	std::string getClassName() const override { return "COMBIntersectSmooth"; }
	void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	COMBIntersectSmooth(std::unique_ptr<SDFObject> objectPtr = nullptr,	double smoothing = DFLT_SMOOTHING, double colorBlending = DFLT_COLORBLENDING, double colorShifting = DFLT_COLORSHIFTING)
		: SmoothCombination(std::move(objectPtr), smoothing, colorBlending, colorShifting) {}
};