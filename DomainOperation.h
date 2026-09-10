#pragma once
#include "TransformOperation.h"

class DomainOperation : public TransformOperation
{
protected:
	Vec3 m_spacing;
public:
	DomainOperation(Vec3 spacing = Vec3::One) : m_spacing(spacing) {};
	SDFOperationType opType() override { return SDFOperationType::Transform; }
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
	friend void SDFOperation::execute(Vec3& p, double& distance, const Color colorIn, Color* colorOut);
};

class RepeatX : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	RepeatX(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
};

class RepeatY : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	RepeatY(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
};

class RepeatZ : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	RepeatZ(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
};

class RepeatXY : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	RepeatXY(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
};

class RepeatXZ : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	RepeatXZ(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
};

class RepeatYZ : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	RepeatYZ(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
};

class RepeatXYZ : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	RepeatXYZ(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
};

class RepeatLimited : public DomainOperation
{
private:
	Vec3 m_limits;
protected:
	void transform(Vec3& p) const override;
public:
	RepeatLimited(Vec3 limits = Vec3::One, Vec3 spacing = Vec3::One) : DomainOperation(spacing), m_limits(limits) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};