#pragma once
#include "SDFOperation.h"

class TransformOperation : public SDFOperation
{
protected:
	void innerUpdate() override {};
public:
	TransformOperation() : SDFOperation(SDFOperationType::Transform) {};
	friend void SDFOperation::execute(Vec3& p, double& distance, const Color colorIn, Color* colorOut);
};

class OPMirrorX : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	OPMirrorX() {};
};

class OPMirrorY : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	OPMirrorY() {};
};

class OPMirrorZ : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	OPMirrorZ() {};
};

class OPMirrorXY : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	OPMirrorXY() {};
};

class OPMirrorXZ : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	OPMirrorXZ() {};
};

class OPMirrorYZ : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	OPMirrorYZ() {};
};

class OPMirrorXYZ : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	OPMirrorXYZ() {};
};

class OPTranslate : public TransformOperation
{
private:
	Vec3 m_translation;
protected:
	void transform(Vec3& p) const override;
public:
	OPTranslate(Vec3 translation = Vec3::Up) : m_translation(translation) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class OPTranslateSine : public TransformOperation
{
private:
	Vec3 m_axis;
	double m_amplitude;
	double m_period;
	double m_phase;
	Vec3 m_updateVal;
protected:
	void innerUpdate() override;
	void transform(Vec3& p) const override;
public:
	OPTranslateSine(Vec3 axis = Vec3::Up, double amplitude = 1.0, double period = 4.0, double phase = 0.0) : m_axis(axis), m_amplitude(amplitude), m_period(period), m_phase(phase) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class OPTranslateLinear : public TransformOperation
{
private:
	Vec3 m_start;
	Vec3 m_end;
	Vec2 m_transitTimes;
	Vec2 m_endDelays;
	double m_phase;
	double m_cycleTime;
	Vec3 m_updateVal;
protected:
	void innerUpdate() override;
	void transform(Vec3& p) const override;
public:
	OPTranslateLinear(Vec3 vector = Vec3::Up, Vec2 transitTimes = 1.0, Vec2 endDelays = 0.0, double phase = 0.0)
		: m_start(vector * -0.5), m_end(vector * 0.5), m_transitTimes(transitTimes)
		, m_endDelays(endDelays), m_phase(phase + transitTimes.x * 0.5)
		, m_cycleTime(m_transitTimes.x + m_transitTimes.y + m_endDelays.x + m_endDelays.y) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class OPCubeRollHeight : public TransformOperation
{
private:
	double m_cubeHeight;
	double m_period;
	double m_phase;
	double m_updateVal{};
protected:
	void innerUpdate() override;
	void transform(Vec3& p) const override;
public:
	OPCubeRollHeight(double cubeHeight = 1.0, double period = 1.0, double phase = 0.0)
		: m_cubeHeight(cubeHeight), m_period(period), m_phase(phase) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class OPRotate : public TransformOperation
{
private:
	Vec3 m_axis;
	double m_rotations;
	Mat3 m_rotMat;
protected:
	void transform(Vec3& p) const override;
public:
	OPRotate(Vec3 axis = Vec3::Up, double rotations = 0.5)
		: m_axis(axis), m_rotations(rotations) {
		Mat3::fromAxisAngle(axis, m_rotMat, rotations * MF::TAU);
	}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class OPRotateContinuous : public TransformOperation
{
private:
	Vec3 m_axis;
	double m_rotationsPerSecond;
	double m_phase;
	Mat3 m_updateVal{};
protected:
	void innerUpdate() override;
	void transform(Vec3& p) const override;
public:
	OPRotateContinuous(Vec3 axis = Vec3::Up, double rotationsPerSecond = 0.25, double phase = 0.0) : m_axis(axis), m_rotationsPerSecond(rotationsPerSecond), m_phase(phase) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class OPRotateScan : public TransformOperation
{
private:
	Vec3 m_axis;
	double m_endAngle;
	Vec2 m_transitTimes;
	Vec2 m_endDelays;
	double m_phase;
	double m_cycleTime;
	Mat3 m_updateVal;
protected:
	void innerUpdate() override;
	void transform(Vec3& p) const override;
public:
	OPRotateScan(Vec3 axis = Vec3::Up, double rotations = 1.0, Vec2 transitTimes = 1.0, Vec2 endDelays = 0.0, double phase = 0.0)
		: m_axis(axis), m_endAngle(rotations* MF::TAU)
		, m_transitTimes(transitTimes), m_endDelays(endDelays), m_phase(phase)
		, m_cycleTime(m_transitTimes.x + m_transitTimes.y + m_endDelays.x + m_endDelays.y) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class OPRotateDelay : public TransformOperation
{
private:
	Vec3 m_axis;
	double m_angle;
	double m_rotateTime;
	double m_cycleTime;
	double m_phase;
	Mat3 m_updateVal;
protected:
	void innerUpdate() override;
	void transform(Vec3& p) const override;
public:
	OPRotateDelay(Vec3 axis = Vec3::Up, double rotations = 1.0, double rotateTime = 1.0, double delayTime = 1.0, double phase = 0.0)
		: m_axis(axis), m_angle(rotations* MF::TAU), m_rotateTime(rotateTime), m_cycleTime(rotateTime + delayTime), m_phase(phase) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class OPCubeRoller : public TransformOperation
{
private:
	std::vector<Vec3> m_axes;
	double m_quarterTurnTime;
	double m_phase;
	Mat3 m_updateVal;
protected:
	void innerUpdate() override;
	void transform(Vec3& p) const override;
public:
	OPCubeRoller(std::vector<Vec3> axes, double quarterTurnTime = 1.0, double phase = 0.0) : m_axes(axes), m_quarterTurnTime(quarterTurnTime), m_phase(phase) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class Elongate : public TransformOperation
{
protected:
	double m_length;
	double m_offset;
	double m_halfLen{};
	double m_halfLenNeg{};
	void innerUpdate() override;
public:
	Elongate(double length = 1.0, double offset = 0.0) : m_length(length), m_offset(offset) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class OPElongateX : public Elongate
{
protected:
	void transform(Vec3& p) const override;
public:
	OPElongateX(double length = 1.0, double offset = 0.0) : Elongate(length, offset) {}
};

class OPElongateY : public Elongate
{
protected:
	void transform(Vec3& p) const override;
public:
	OPElongateY(double length = 1.0, double offset = 0.0) : Elongate(length, offset) {}
};

class OPElongateZ : public Elongate
{
protected:
	void transform(Vec3& p) const override;
public:
	OPElongateZ(double length = 1.0, double offset = 0.0) : Elongate(length, offset) {}
};