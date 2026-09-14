#pragma once
#include "SDFOperation.h"

class DistortOperation : public SDFOperation
{
protected:
	double m_magnitude;
	virtual void transform(Vec3& p) const = 0;
	virtual std::string getClassName() const = 0;
public:
	DistortOperation(double magnitude) : SDFOperation(SDFOperationType::Transform), m_magnitude(magnitude) {};
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
	void serialize(std::vector<std::string>& strVec, size_t level) const override;
	friend void SDFOperation::execute(Vec3& p, double& distance, const Color colorIn, Color* colorOut);
};

class OPTwist : public DistortOperation
{
protected:
	void innerUpdate() override {};
	void transform(Vec3& p) const override;
	std::string getClassName() const override { return "OPTwist"; }
public:
	OPTwist(double magnitude = 1.0) : DistortOperation(magnitude) {}
};

class OPBend : public DistortOperation
{
protected:
	void innerUpdate() override {};
	void transform(Vec3& p) const override;
	std::string getClassName() const override { return "OPBend"; }
public:
	OPBend(double magnitude = 1.0) : DistortOperation(magnitude) {}
};