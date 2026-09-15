#pragma once
#include "SDFDefaults.h"
#include "TransformOperation.h"

class DomainOperation : public TransformOperation
{
protected:
	Vec3 m_spacing;
	virtual std::string getClassName() const = 0;
public:
	DomainOperation(Vec3 spacing = DEF::DOMSpacing) : m_spacing(spacing) {};
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
	virtual void serialize(std::vector<std::string>& strVec, size_t level) const override;
	friend void SDFOperation::execute(Vec3& p, double& distance, const Color colorIn, Color* colorOut);
};

class OPRepeatX : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
	std::string getClassName() const override { return "OPRepeatX"; }
public:
	OPRepeatX(Vec3 spacing = DEF::DOMSpacing) : DomainOperation(spacing) {}
};

class OPRepeatY : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
	std::string getClassName() const override { return "OPRepeatY"; }
public:
	OPRepeatY(Vec3 spacing = DEF::DOMSpacing) : DomainOperation(spacing) {}
};

class OPRepeatZ : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
	std::string getClassName() const override { return "OPRepeatZ"; }
public:
	OPRepeatZ(Vec3 spacing = DEF::DOMSpacing) : DomainOperation(spacing) {}
};

class OPRepeatXY : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
	std::string getClassName() const override { return "OPRepeatXY"; }
public:
	OPRepeatXY(Vec3 spacing = DEF::DOMSpacing) : DomainOperation(spacing) {}
};

class OPRepeatXZ : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
	std::string getClassName() const override { return "OPRepeatXZ"; }
public:
	OPRepeatXZ(Vec3 spacing = DEF::DOMSpacing) : DomainOperation(spacing) {}
};

class OPRepeatYZ : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
	std::string getClassName() const override { return "OPRepeatYZ"; }
public:
	OPRepeatYZ(Vec3 spacing = DEF::DOMSpacing) : DomainOperation(spacing) {}
};

class OPRepeatXYZ : public DomainOperation
{
protected:
	void transform(Vec3& p) const override;
	std::string getClassName() const override { return "OPRepeatXYZ"; }
public:
	OPRepeatXYZ(Vec3 spacing = DEF::DOMSpacing) : DomainOperation(spacing) {}
};

class OPRepeatLimited : public DomainOperation
{
private:
	Vec3 m_limits;
protected:
	void transform(Vec3& p) const override;
	void serialize(std::vector<std::string>& strVec, size_t level) const override;
	std::string getClassName() const override { return "OPRepeatLimited"; }
public:
	OPRepeatLimited(Vec3 limits = DEF::DOMLimits, Vec3 spacing = DEF::DOMSpacing) : DomainOperation(spacing), m_limits(limits) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};