#pragma once
#include "Enumerations.h"
#include "TransformOperation.h"

// Domain operations are a very cheap way to repeat objects through domain repetitions (repeating objects throughout a grid or other similar construct)
class DomainOperation : public TransformOperation
{
public:
	inline static const Vec3 DFLT_SPACING{ 1.0 };
protected:
	Vec3 getPForAxis(Vec3 p, Axis axis) const;
	std::string getPSwizzleForAxis(Axis axis) const;
	Vec3 m_spacing;
	virtual std::string getClassName() const = 0;
	virtual void innerSerialize(std::vector<std::string>& strVec, size_t level) const override;
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const = 0;
	DomainOperation(Vec3 spacing = DFLT_SPACING) : m_spacing(spacing) {};
public:
	BindResult bindAnimation(std::unique_ptr<Animation> animation) override;
	virtual ~DomainOperation() = default;
};

// Repeat the object throughout a 3D grid.  Extents of repetition can be limited
class OPRepeat : public DomainOperation
{
public:
	inline static const Vec3 DFLT_LIMITS{ 1.0 };
private:
	Vec3 m_limits;
protected:
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const override;
	void innerSerialize(std::vector<std::string>& strVec, size_t level) const override;
	std::string getClassName() const override { return "OPRepeat"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPRepeat(Vec3 limits = DFLT_LIMITS, Vec3 spacing = DFLT_SPACING) : DomainOperation(spacing), m_limits(limits) {}
	BindResult bindAnimation(std::unique_ptr<Animation> animation) override;
};

// Repeat an object around a circle. Naive operation, requires rotational symmetry or artifacting may occur
class OPRepeatCircle : public DomainOperation 
{
public:
	static inline const size_t DFLT_REPETITIONS{ 10 };
private:
	Axis m_axis;
	size_t m_repetitions;
	double m_repetitionsD;	// animatable copy of m_repetitions, as used by execute and the shader
protected:
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const override;
	void innerSerialize(std::vector<std::string>& strVec, size_t level) const override;
	std::string getClassName() const override { return "OPRepeatCircle"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPRepeatCircle(Axis axis, size_t repetitions = DFLT_REPETITIONS) : DomainOperation(), m_axis(axis), m_repetitions(repetitions), m_repetitionsD(static_cast<double>(repetitions)) {}
	BindResult bindAnimation(std::unique_ptr<Animation> animation) override;
};

// Repeat an object by mirroring over an axis
class OPMirror : public TransformOperation
{
private:
	bool m_mirrorX{ true };
	bool m_mirrorY{ true };
	bool m_mirrorZ{ true };
	std::string getSwizzleForMirror() const;
protected:
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const;
	std::string getClassName() const { return "OPMirror"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
	void innerInnerSerialize(std::stringstream& ss) const override;
public:
	OPMirror(bool mirrorX = true, bool mirrorY = false, bool mirrorZ = false)
		: m_mirrorX(mirrorX), m_mirrorY(mirrorY), m_mirrorZ(mirrorZ) {};
};
