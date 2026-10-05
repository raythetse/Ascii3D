#pragma once
#include "SDFOperation.h"

class ResurfaceOperation : public SDFOperation
{
public:
	inline static const double DFLT_MAGNITUDE{ 0.01 };
protected:
	double m_magnitude{};
	void innerUpdate() override {};
	virtual std::string getClassName() const = 0;
	void innerSerialize(std::vector<std::string>& strVec, size_t level) const override;
public:
	ResurfaceOperation(double magnitude) : SDFOperation(SDFOperationType::Resurface), m_magnitude(magnitude) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
	friend void SDFOperation::execute(Vec3& p, double& distance, const Color colorIn, Color* colorOut);
};

class OPRoundOff : public ResurfaceOperation
{
protected:
	double resurface(double distance) override;
	std::string getClassName() const { return "OPRoundOff"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	OPRoundOff(double magnitude = DFLT_MAGNITUDE) : ResurfaceOperation(magnitude) {}
};

class OPOnion : public ResurfaceOperation
{
protected:
	double resurface(double distance) override;
	std::string getClassName() const { return "OPOnion"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	OPOnion(double magnitude = DFLT_MAGNITUDE) : ResurfaceOperation(magnitude) {}
};

class OPInsideOut : public ResurfaceOperation
{
protected:
	double resurface(double distance) override;
	std::string getClassName() const { return "OPOnion"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	OPInsideOut() : ResurfaceOperation(0.0) {}
};
