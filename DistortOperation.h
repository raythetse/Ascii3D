#pragma once
#include "SDFOperation.h"

class DistortOperation : public SDFOperation
{
protected:
	double m_magnitude;
	virtual void transform(Vec3& p) const = 0;
public:
	DistortOperation(double magnitude) : SDFOperation(SDFOperationType::Transform), m_magnitude(magnitude) {};
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
	friend void SDFOperation::execute(Vec3& p, double& distance, const Color colorIn, Color* colorOut);
};

class OPTwist : public DistortOperation
{
protected:
	void innerUpdate() override {};
	void transform(Vec3& p) const override;
public:
	OPTwist(double turns = 1.0) : DistortOperation(turns) {}
};

class OPBend : public DistortOperation
{
protected:
	void innerUpdate() override {};
	void transform(Vec3& p) const override;
public:
	OPBend(double turns = 1.0) : DistortOperation(turns) {}
};