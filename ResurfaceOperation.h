#pragma once

#include "SDFOperation.h"

class ResurfaceOperation : public SDFOperation
{
protected:
	double m_magnitude{};
	double m_updateValue;
	void innerUpdate() override {};
public:
	ResurfaceOperation(double magnitude = 0.01) : m_magnitude(magnitude) {}
	SDFOperationType opType() override { return SDFOperationType::Resurface; }
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
	friend void SDFOperation::execute(Vec3& p, double& distance, const Color colorIn, Color* colorOut);
};

class RoundOff : public ResurfaceOperation
{
protected:
	double resurface(double distance) override;
public:
	RoundOff(double magnitude = 0.1) : ResurfaceOperation(magnitude) {}
};

class Onion : public ResurfaceOperation
{
protected:
	double resurface(double distance) override;
public:
	Onion(double magnitude = 0.01) : ResurfaceOperation(magnitude) {}
};
