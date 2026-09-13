#pragma once
#include "TransformOperation.h"

class DomainOperation : public TransformOperation
{
protected:
	Vec3 m_spacing;
public:
	DomainOperation(Vec3 spacing = Vec3::One) : m_spacing(spacing) {};
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
	friend void SDFOperation::execute(Vec3& p, double& distance, const Color colorIn, Color* colorOut);
};

class OPRepeatX : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	OPRepeatX(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
};

class OPRepeatY : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	OPRepeatY(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
};

class OPRepeatZ : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	OPRepeatZ(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
};

class OPRepeatXY : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	OPRepeatXY(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
};

class OPRepeatXZ : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	OPRepeatXZ(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
};

class OPRepeatYZ : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	OPRepeatYZ(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
};

class OPRepeatXYZ : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	OPRepeatXYZ(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
};

class OPRepeatLimited : public DomainOperation
{
private:
	Vec3 m_limits;
protected:
	void transform(Vec3& p) const override;
public:
	OPRepeatLimited(Vec3 limits = Vec3::One, Vec3 spacing = Vec3::One) : DomainOperation(spacing), m_limits(limits) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};