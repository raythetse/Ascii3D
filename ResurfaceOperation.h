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
	ResurfaceOperation(double magnitude) : m_magnitude(magnitude) {}
public:
	void bindAnimation(std::unique_ptr<Animation> animation) override;
};

class OPRoundOff : public ResurfaceOperation
{
protected:
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const;
	std::string getClassName() const { return "OPRoundOff"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPRoundOff(double magnitude = DFLT_MAGNITUDE) : ResurfaceOperation(magnitude) {}
};

class OPOnion : public ResurfaceOperation
{
protected:
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const;
	std::string getClassName() const { return "OPOnion"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPOnion(double magnitude = DFLT_MAGNITUDE) : ResurfaceOperation(magnitude) {}
};

class OPInsideOut : public ResurfaceOperation
{
protected:
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const;
	std::string getClassName() const { return "OPOnion"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPInsideOut() : ResurfaceOperation(0.0) {}
};
