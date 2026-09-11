#include "TransformOperation.h"
#include "GameTimer.h"
#include <algorithm>

void MirrorX::transform(Vec3& p) const
{
	p.x = abs(p.x);
}

void MirrorY::transform(Vec3& p) const
{
	p.y = abs(p.x);
}

void MirrorZ::transform(Vec3& p) const
{
	p.z = abs(p.z);
}

void MirrorXY::transform(Vec3& p) const
{
	p.x = abs(p.x);
	p.y = abs(p.y);
}

void MirrorXZ::transform(Vec3& p) const
{
	p.x = abs(p.x);
	p.z = abs(p.z);
}

void MirrorYZ::transform(Vec3& p) const
{
	p.y = abs(p.y);
	p.z = abs(p.z);
}

void MirrorXYZ::transform(Vec3& p) const
{
	p = p.abs();
}

void Translate::transform(Vec3& p) const
{
	p -= m_translation;
}

void Translate::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "translation") {
		propMod->BindToProp(&m_translation);
		m_propMods.push_back(std::move(propMod));
	}
}

void TranslateSine::innerUpdate() {
	m_updateVal = m_axis * m_amplitude * sin(GameTimer::gameTime() / m_period * MF::TAU + m_phase * MF::TAU);
}

void TranslateSine::transform(Vec3& p) const
{
	p -= m_updateVal;
}

void TranslateSine::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "amplitude") {
		propMod->BindToProp(&m_amplitude);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "axis") {
		propMod->BindToProp(&m_axis);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "period") {
		propMod->BindToProp(&m_period);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "phase") {
		propMod->BindToProp(&m_phase);
		m_propMods.push_back(std::move(propMod));
	}
}

void TranslateLinear::innerUpdate() {
	double time{ std::fmod(GameTimer::gameTime() + m_phase, m_cycleTime) };
	if (time <= m_transitTimes.x) {
		time /= m_transitTimes.x;
		m_updateVal = m_start.lerp(m_end, time);
		return;
	}
	double timeFrame = m_transitTimes.x + m_endDelays.x;
	if (time <= timeFrame) {
		m_updateVal = m_end;
		return;
	}
	timeFrame += m_transitTimes.y;
	if (time <= timeFrame) {
		time = (time - m_transitTimes.x - m_endDelays.x) / m_transitTimes.y;
		m_updateVal = m_end.lerp(m_start, time);
		return;
	}
	m_updateVal = m_start;
}

void TranslateLinear::transform(Vec3& p) const
{
	p -= m_updateVal;
}

void TranslateLinear::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "cycleTime") {
		propMod->BindToProp(&m_cycleTime);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "end") {
		propMod->BindToProp(&m_end);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "endDelays") {
		propMod->BindToProp(&m_endDelays);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "phase") {
		propMod->BindToProp(&m_phase);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "start") {
		propMod->BindToProp(&m_start);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "transitTimes") {
		propMod->BindToProp(&m_transitTimes);
		m_propMods.push_back(std::move(propMod));
	}
}

void CubeRollHeight::innerUpdate() {
	double angle = std::fmod(GameTimer::gameTime() + m_phase, m_period) * MF::HALFPI - MF::HALFPI;
	Vec2 rot = Vec2{ -m_cubeHeight * 0.5, m_cubeHeight * 0.5 }.rotate(angle);
	m_updateVal = (rot.y - m_cubeHeight * 0.5);
}

void CubeRollHeight::transform(Vec3& p) const
{
	double angle = std::fmod(GameTimer::gameTime() + m_phase, m_period) * MF::HALFPI - MF::HALFPI;
	Vec2 rot = Vec2{ -m_cubeHeight * 0.5, m_cubeHeight * 0.5 }.rotate(angle);
	p.y -= m_updateVal;
}

void CubeRollHeight::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "cubeHeight") {
		propMod->BindToProp(&m_cubeHeight);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "period") {
		propMod->BindToProp(&m_period);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "phase") {
		propMod->BindToProp(&m_phase);
		m_propMods.push_back(std::move(propMod));
	}
}

void Rotate::transform(Vec3& p) const
{
	p = m_rotMat * p;
}

void Rotate::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "axis") {
		propMod->BindToProp(&m_axis);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "rotations") {
		propMod->BindToProp(&m_rotations);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "rotMat") {
		propMod->BindToProp(&m_rotMat);
		m_propMods.push_back(std::move(propMod));
	}
}
void RotateContinuous::innerUpdate() {
	Mat3::fromAxisAngle(m_axis, m_updateVal, (GameTimer::gameTime() + (m_phase / m_rotationsPerSecond)) * m_rotationsPerSecond * MF::TAU);
}

void RotateContinuous::transform(Vec3& p) const
{
	p = m_updateVal * p;
}

void RotateContinuous::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "axis") {
		propMod->BindToProp(&m_axis);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "phase") {
		propMod->BindToProp(&m_phase);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "rotationsPerSecond") {
		propMod->BindToProp(&m_rotationsPerSecond);
		m_propMods.push_back(std::move(propMod));
	}
}

void RotateScan::innerUpdate() {
	double time{ std::fmod(GameTimer::gameTime() + m_phase, m_cycleTime) };
	if (time <= m_transitTimes.x) {
		time /= m_transitTimes.x;
		Mat3::fromAxisAngle(m_axis, m_updateVal, m_endAngle * time);
		return;
	}
	double timeFrame = m_transitTimes.x + m_endDelays.x;
	if (time <= timeFrame) {
		Mat3::fromAxisAngle(m_axis, m_updateVal, m_endAngle);
		return;
	}
	timeFrame += m_transitTimes.y;
	if (time <= timeFrame) {
		time = (time - (m_transitTimes.x + m_endDelays.x)) / m_transitTimes.y;
		Mat3::fromAxisAngle(m_axis, m_updateVal, m_endAngle * (1.0 - time));
		return;
	}
	m_updateVal = Mat3::Identity;
}

void RotateScan::transform(Vec3& p) const
{
	p = m_updateVal * p;
}

void RotateScan::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "axis") {
		propMod->BindToProp(&m_axis);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "cycleTime") {
		propMod->BindToProp(&m_cycleTime);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "endAngle") {
		propMod->BindToProp(&m_endAngle);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "endDelays") {
		propMod->BindToProp(&m_endDelays);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "phase") {
		propMod->BindToProp(&m_phase);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "transitTimes") {
		propMod->BindToProp(&m_transitTimes);
		m_propMods.push_back(std::move(propMod));
	}
}

void RotateDelay::innerUpdate() {
	double completeCycles{ std::floor((GameTimer::gameTime() + m_phase) / m_cycleTime) };
	double time{ std::fmod(GameTimer::gameTime() + m_phase, m_cycleTime) };
	completeCycles += std::min(m_rotateTime, time) / m_rotateTime;
	Mat3::fromAxisAngle(m_axis, m_updateVal, completeCycles * m_angle);
}

void RotateDelay::transform(Vec3& p) const
{
	p = m_updateVal * p;
}

void RotateDelay::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "angle") {
		propMod->BindToProp(&m_angle);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "axis") {
		propMod->BindToProp(&m_axis);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "cycleTime") {
		propMod->BindToProp(&m_cycleTime);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "phase") {
		propMod->BindToProp(&m_phase);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "rotateTime") {
		propMod->BindToProp(&m_rotateTime);
		m_propMods.push_back(std::move(propMod));
	}
}

void CubeRoller::innerUpdate() {
	double cycleTime = m_axes.size() * m_quarterTurnTime;
	double time = std::fmod(GameTimer::gameTime() + m_phase, cycleTime);
	size_t index = static_cast<size_t>(time);
	time /= m_quarterTurnTime;
	Mat3::fromAxisAngle(m_axes[index], m_updateVal, MF::HALFPI * time);
}

void CubeRoller::transform(Vec3& p) const
{
	p = m_updateVal * p;
}

void CubeRoller::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "axes") {
		propMod->BindToProp(&m_axes);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "phase") {
		propMod->BindToProp(&m_phase);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "quarterTurnTime") {
		propMod->BindToProp(&m_quarterTurnTime);
		m_propMods.push_back(std::move(propMod));
	}
}

void Elongate::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "length") {
		propMod->BindToProp(&m_length);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "offset") {
		propMod->BindToProp(&m_offset);
		m_propMods.push_back(std::move(propMod));
	}
}

void ElongateX::transform(Vec3& p) const
{
	double half = m_length * 0.5;
	p.x -= std::clamp(p.x - m_offset, -half, half);
}

void ElongateY::transform(Vec3& p) const
{
	double half = m_length * 0.5;
	p.y -= std::clamp(p.y - m_offset, -half, half);
}

void ElongateZ::transform(Vec3& p) const
{
	double half = m_length * 0.5;
	p.z -= std::clamp(p.z - m_offset, -half, half);
}
