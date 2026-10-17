#pragma once
#include "TransformOperation.h"

class DomainOperation : public TransformOperation
{
public:
	inline static const Vec3 DFLT_SPACING{ 1.0 };
protected:
	Vec3 m_spacing;
	virtual std::string getClassName() const = 0;
	virtual void innerSerialize(std::vector<std::string>& strVec, size_t level) const override;
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const = 0;
	DomainOperation(Vec3 spacing = DFLT_SPACING) : m_spacing(spacing) {};
public:
	void bindAnimation(std::unique_ptr<Animation> animation) override;
};

class OPRepeatX : public DomainOperation
{
protected:
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const override;
	std::string getClassName() const override { return "OPRepeatX"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPRepeatX(Vec3 spacing = DFLT_SPACING) : DomainOperation(spacing) {}
};

class OPRepeatY : public DomainOperation
{
protected:
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const override;
	std::string getClassName() const override { return "OPRepeatY"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPRepeatY(Vec3 spacing = DFLT_SPACING) : DomainOperation(spacing) {}
};

class OPRepeatZ : public DomainOperation
{
protected:
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const override;
	std::string getClassName() const override { return "OPRepeatZ"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPRepeatZ(Vec3 spacing = DFLT_SPACING) : DomainOperation(spacing) {}
};

class OPRepeatXY : public DomainOperation
{
protected:
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const override;
	std::string getClassName() const override { return "OPRepeatXY"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPRepeatXY(Vec3 spacing = DFLT_SPACING) : DomainOperation(spacing) {}
};

class OPRepeatXZ : public DomainOperation
{
protected:
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const override;
	std::string getClassName() const override { return "OPRepeatXZ"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPRepeatXZ(Vec3 spacing = DFLT_SPACING) : DomainOperation(spacing) {}
};

class OPRepeatYZ : public DomainOperation
{
protected:
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const override;
	std::string getClassName() const override { return "OPRepeatYZ"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPRepeatYZ(Vec3 spacing = DFLT_SPACING) : DomainOperation(spacing) {}
};

class OPRepeatXYZ : public DomainOperation
{
protected:
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const override;
	std::string getClassName() const override { return "OPRepeatXYZ"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPRepeatXYZ(Vec3 spacing = DFLT_SPACING) : DomainOperation(spacing) {}
};

class OPRepeatLimited : public DomainOperation
{
public:
	inline static const Vec3 DFLT_LIMITS{ 1.0 };
private:
	Vec3 m_limits;
protected:
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const override;
	void innerSerialize(std::vector<std::string>& strVec, size_t level) const override;
	std::string getClassName() const override { return "OPRepeatLimited"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPRepeatLimited(Vec3 limits = DFLT_LIMITS, Vec3 spacing = DFLT_SPACING) : DomainOperation(spacing), m_limits(limits) {}
	void bindAnimation(std::unique_ptr<Animation> animation) override;
};