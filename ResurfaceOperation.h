#pragma once
#include "SDFDefaults.h"
#include "SDFOperation.h"

class ResurfaceOperation : public SDFOperation
{
protected:
	double m_magnitude{};
	double m_updateValue{};
	void innerUpdate() override {};
	virtual std::string getClassName() const = 0;
public:
	ResurfaceOperation(double magnitude) : SDFOperation(SDFOperationType::Resurface), m_magnitude(magnitude) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
	void serialize(std::vector<std::string>& strVec, size_t level) const override;
	friend void SDFOperation::execute(Vec3& p, double& distance, const Color colorIn, Color* colorOut);
};

class OPRoundOff : public ResurfaceOperation
{
protected:
	double resurface(double distance) override;
	std::string getClassName() const { return "OPRoundOff"; }
public:
	OPRoundOff(double magnitude = DEF::RESURFRoundMagnitude) : ResurfaceOperation(magnitude) {}
};

class OPOnion : public ResurfaceOperation
{
protected:
	double resurface(double distance) override;
	std::string getClassName() const { return "OPOnion"; }
public:
	OPOnion(double magnitude = DEF::RESURFOnionMagnitude) : ResurfaceOperation(magnitude) {}
};
