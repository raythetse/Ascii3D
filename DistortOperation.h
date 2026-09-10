#pragma once
#include "SDFOperation.h"

class DistortOperation : public SDFOperation
{
protected:
	double m_magnitude;
	virtual void transform(Vec3& p) const = 0;
public:
	DistortOperation(double magnitude) : m_magnitude(magnitude) {};
	SDFOperationType opType() override { return SDFOperationType::Transform; }
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
	friend void SDFOperation::execute(Vec3& p, double& distance, const Color colorIn, Color* colorOut);
};

class Twist : public DistortOperation
{
protected:
	void innerUpdate() override {};
	void transform(Vec3& p) const override;
public:
	Twist(double turns = 1.0) : DistortOperation(turns) {}
};

class Bend : public DistortOperation
{
protected:
	void innerUpdate() override {};
	void transform(Vec3& p) const override;
public:
	Bend(double turns = 1.0) : DistortOperation(turns) {}
};