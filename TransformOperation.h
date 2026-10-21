#pragma once
#include "SDFOperation.h"

class TransformOperation : public SDFOperation
{
protected:
	void innerUpdate() override {};
	virtual std::string getClassName() const = 0;
	virtual void innerInnerSerialize(std::stringstream& ss) const {};
	void innerSerialize(std::vector<std::string>& strVec, size_t level) const override;
	TransformOperation() = default;
public:
	void bindAnimation(std::unique_ptr<Animation> animation) override {};
};

class OPMirror : public TransformOperation
{
private:
	bool m_mirrorX{ true };
	bool m_mirrorY{ true };
	bool m_mirrorZ{ true };
	std::string getSwizzle() const;
protected:
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const;
	std::string getClassName() const { return "OPMirror"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
	void innerInnerSerialize(std::stringstream& ss) const override;
public:
	OPMirror(bool mirrorX = true, bool mirrorY = false, bool mirrorZ = false)
		: m_mirrorX(mirrorX), m_mirrorY(mirrorY), m_mirrorZ(mirrorZ) {};
};

class OPTranslate : public TransformOperation
{
public:
	inline static const Vec3 DFLT_TRANSLATION{ 0.0, 1.0, 0.0 };
private:
	Vec3 m_translation;
protected:
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const;
	std::string getClassName() const { return "OPTranslate"; }
	void innerInnerSerialize(std::stringstream& ss) const override;
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPTranslate(Vec3 translation = DFLT_TRANSLATION)
		: m_translation(translation) {}
	void bindAnimation(std::unique_ptr<Animation> animation) override;
};

class OPRotate : public TransformOperation
{
public:
	inline static const Vec3 DFLT_AXIS{ 0.0, 1.0, 0.0 };
	inline static const double DFLT_ROTATIONS{ 0.5 };
private:
	Vec3 m_axis;
	double m_rotations;
	Mat3 m_rotMat;
protected:
	void innerUpdate() override;
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const;
	std::string getClassName() const { return "OPRotate"; }
	void innerInnerSerialize(std::stringstream& ss) const override;
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPRotate(Vec3 axis = DFLT_AXIS, double rotations = DFLT_ROTATIONS)
		: m_axis(axis), m_rotations(rotations) {}
	void bindAnimation(std::unique_ptr<Animation> animation) override;
};

class OPCubeRollHeight : public TransformOperation
{
public:
	inline static const double DFLT_CUBEHEIGHT{ 1.0 };
	inline static const double DFLT_PERIOD{ 1.0 };
	inline static const double DFLT_PHASE{ 0.0 };
private:
	double m_cubeHeight;
	double m_period;
	double m_phase;
	double m_updateVal{};
protected:
	void innerUpdate() override;
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const;
	std::string getClassName() const { return "OPCubeRollHeight"; }
	void innerInnerSerialize(std::stringstream& ss) const override;
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPCubeRollHeight(double cubeHeight = DFLT_CUBEHEIGHT, double period = DFLT_PERIOD, double phase = DFLT_PHASE)
		: m_cubeHeight(cubeHeight), m_period(period), m_phase(phase) {}
	void bindAnimation(std::unique_ptr<Animation> animation) override;
};

class OPCubeRoller : public TransformOperation
{
public:
	inline static std::vector<Vec3>	DFLT_AXES{ Vec3::Forward, Vec3::Right, Vec3::Backward, Vec3::Left };
	inline static const double DFLT_QUARTERTURNTIME{ 0.5 };
	inline static const double DFLT_PHASE{ 0.0 };
private:
	std::vector<Vec3> m_axes;
	double m_quarterTurnTime;
	double m_phase;
	Mat3 m_updateVal;
protected:
	void innerUpdate() override;
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const;
	std::string getClassName() const { return "OPCubeRoller"; }
	void innerInnerSerialize(std::stringstream& ss) const override;
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPCubeRoller(std::vector<Vec3> axes = DFLT_AXES, double quarterTurnTime = DFLT_QUARTERTURNTIME, double phase = DFLT_PHASE)
		: m_axes(axes), m_quarterTurnTime(quarterTurnTime), m_phase(phase) {}
	void bindAnimation(std::unique_ptr<Animation> animation) override;
};

class OPElongate : public TransformOperation
{
private:
	double executeTerm(double d) const;
public:
	inline static const double DFLT_LENGTH{ 1.0 };
	inline static const double DFLT_OFFSET{ 0.0 };
protected:
	Axis m_axis;
	double m_length;
	double m_offset;
	double m_halfLen{};
	void innerUpdate() override;
	std::string getClassName() const { return "OPElongate"; }
	void innerInnerSerialize(std::stringstream& ss) const override;
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const;
public:
	OPElongate(Axis axis = AXIS_Y, double length = DFLT_LENGTH, double offset = DFLT_OFFSET)
		: m_axis(axis), m_length(length), m_offset(offset) {}
	void bindAnimation(std::unique_ptr<Animation> animation) override;
};