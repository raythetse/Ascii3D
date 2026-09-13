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

class MirrorX : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	MirrorX() {};
};

class MirrorY : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	MirrorY() {};
};

class MirrorZ : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	MirrorZ() {};
};

class MirrorXY : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	MirrorXY() {};
};

class MirrorXZ : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	MirrorXZ() {};
};

class MirrorYZ : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	MirrorYZ() {};
};

class MirrorXYZ : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
public:
	MirrorXYZ() {};
};

class Translate : public TransformOperation
{
private:
	Vec3 m_translation;
protected:
	void transform(Vec3& p) const override;
public:
	Translate(Vec3 translation = Vec3::Up) : m_translation(translation) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class TranslateSine : public TransformOperation
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
	TranslateSine(Vec3 axis = Vec3::Up, double amplitude = 1.0, double period = 4.0, double phase = 0.0) : m_axis(axis), m_amplitude(amplitude), m_period(period), m_phase(phase) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class TranslateLinear : public TransformOperation
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
	TranslateLinear(Vec3 vector = Vec3::Up, Vec2 transitTimes = 1.0, Vec2 endDelays = 0.0, double phase = 0.0)
		: m_start(vector * -0.5), m_end(vector * 0.5), m_transitTimes(transitTimes)
		, m_endDelays(endDelays), m_phase(phase + transitTimes.x * 0.5)
		, m_cycleTime(m_transitTimes.x + m_transitTimes.y + m_endDelays.x + m_endDelays.y) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class CubeRollHeight : public TransformOperation
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
	CubeRollHeight(double cubeHeight = 1.0, double period = 1.0, double phase = 0.0)
		: m_cubeHeight(cubeHeight), m_period(period), m_phase(phase) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class Rotate : public TransformOperation
{
private:
	Vec3 m_axis;
	double m_rotations;
	Mat3 m_rotMat;
protected:
	void transform(Vec3& p) const override;
public:
	Rotate(Vec3 axis = Vec3::Up, double rotations = 0.5)
		: m_axis(axis), m_rotations(rotations) {
		Mat3::fromAxisAngle(axis, m_rotMat, rotations * MF::TAU);
	}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class RotateContinuous : public TransformOperation
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
	RotateContinuous(Vec3 axis = Vec3::Up, double rotationsPerSecond = 0.25, double phase = 0.0) : m_axis(axis), m_rotationsPerSecond(rotationsPerSecond), m_phase(phase) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class RotateScan : public TransformOperation
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
	RotateScan(Vec3 axis = Vec3::Up, double rotations = 1.0, Vec2 transitTimes = 1.0, Vec2 endDelays = 0.0, double phase = 0.0)
		: m_axis(axis), m_endAngle(rotations* MF::TAU)
		, m_transitTimes(transitTimes), m_endDelays(endDelays), m_phase(phase)
		, m_cycleTime(m_transitTimes.x + m_transitTimes.y + m_endDelays.x + m_endDelays.y) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class RotateDelay : public TransformOperation
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
	RotateDelay(Vec3 axis = Vec3::Up, double rotations = 1.0, double rotateTime = 1.0, double delayTime = 1.0, double phase = 0.0)
		: m_axis(axis), m_angle(rotations* MF::TAU), m_rotateTime(rotateTime), m_cycleTime(rotateTime + delayTime), m_phase(phase) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class CubeRoller : public TransformOperation
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
	CubeRoller(std::vector<Vec3> axes, double quarterTurnTime = 1.0, double phase = 0.0) : m_axes(axes), m_quarterTurnTime(quarterTurnTime), m_phase(phase) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class Elongate : public TransformOperation
{
protected:
	double m_length;
	double m_offset;
	double m_updateVal{};
	double m_updateValNeg{};
	void innerUpdate() override;
public:
	Elongate(double length = 1.0, double offset = 0.0) : m_length(length), m_offset(offset) {}
	void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) override;
};

class ElongateX : public Elongate
{
protected:
	void transform(Vec3& p) const override;
public:
	ElongateX(double length = 1.0, double offset = 0.0) : Elongate(length, offset) {}
};

class ElongateY : public Elongate
{
protected:
	void transform(Vec3& p) const override;
public:
	ElongateY(double length = 1.0, double offset = 0.0) : Elongate(length, offset) {}
};

class ElongateZ : public Elongate
{
protected:
	void transform(Vec3& p) const override;
public:
	ElongateZ(double length = 1.0, double offset = 0.0) : Elongate(length, offset) {}
};