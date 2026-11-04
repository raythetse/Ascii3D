#pragma once
#include "SDFOperation.h"

// Moves an object around in the scene
class TransformOperation : public SDFOperation
{
protected:
	void innerUpdate() override {};
	virtual std::string getClassName() const = 0;
	virtual void innerInnerSerialize(std::stringstream& ss) const {};
	void innerSerialize(std::vector<std::string>& strVec, size_t level) const override;
	TransformOperation() = default;
public:
	BindResult bindAnimation(std::unique_ptr<Animation> animation) override { return BindResult::UnknownProperty; }
	virtual ~TransformOperation() = default;
};

// Moves the position of the object
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
	BindResult bindAnimation(std::unique_ptr<Animation> animation) override;
};

// Rotates the objects
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
	BindResult bindAnimation(std::unique_ptr<Animation> animation) override;
};

// Creates a bouncing effect specifically for rolling a cube up and over its edges
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
	BindResult bindAnimation(std::unique_ptr<Animation> animation) override;
};

// Controls a cube rolling around along axes.
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
	BindResult bindAnimation(std::unique_ptr<Animation> animation) override;
};

// Elongates and object along an axis by moving two halves in opposite directions.  For example, can turn a sphere into a capsule.
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
	BindResult bindAnimation(std::unique_ptr<Animation> animation) override;
};