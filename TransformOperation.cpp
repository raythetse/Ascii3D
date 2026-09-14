#include "TransformOperation.h"
#include "GameTimer.h"
#include <algorithm>
#include <sstream>

void TransformOperation::serialize(std::vector<std::string>& strVec, size_t level) const
{
	std::stringstream ss{};
	ss << std::string(level, '\t');
	ss << getClassName();
	serializeInner(ss);
	strVec.push_back(ss.str());
}

void OPMirrorX::transform(Vec3& p) const
{
	p.x = abs(p.x);
}

void OPMirrorY::transform(Vec3& p) const
{
	p.y = abs(p.x);
}

void OPMirrorZ::transform(Vec3& p) const
{
	p.z = abs(p.z);
}

void OPMirrorXY::transform(Vec3& p) const
{
	p.x = abs(p.x);
	p.y = abs(p.y);
}

void OPMirrorXZ::transform(Vec3& p) const
{
	p.x = abs(p.x);
	p.z = abs(p.z);
}

void OPMirrorYZ::transform(Vec3& p) const
{
	p.y = abs(p.y);
	p.z = abs(p.z);
}

void OPMirrorXYZ::transform(Vec3& p) const
{
	p = p.abs();
}

void OPTranslate::transform(Vec3& p) const
{
	p -= m_translation;
}

void OPTranslate::serializeInner(std::stringstream& ss) const
{
	ss << ",m_translation," << m_translation;
}

void OPTranslate::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "translation") {
		propMod->BindToProp(&m_translation);
		m_propMods.push_back(std::move(propMod));
	}
}

void OPTranslateSine::innerUpdate() {
	m_updateVal = m_axis * m_amplitude * sin(GameTimer::gameTime() / m_period * MF::TAU + m_phase * MF::TAU);
}

void OPTranslateSine::transform(Vec3& p) const
{
	p -= m_updateVal;
}

void OPTranslateSine::serializeInner(std::stringstream& ss) const
{
	ss << ",m_axis," << m_axis.x << ',' << m_axis.y << ',' << m_axis.z;
	ss << ",m_amplitude," << m_amplitude;
	ss << ",m_period," << m_period;
	ss << ",m_phase," << m_phase;
}

void OPTranslateSine::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
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

void OPTranslateLinear::innerUpdate() {
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

void OPTranslateLinear::transform(Vec3& p) const
{
	p -= m_updateVal;
}

void OPTranslateLinear::serializeInner(std::stringstream& ss) const
{
	ss << ",m_vector," << m_vector.x << m_vector.y << ',' << m_vector.z;
	ss << ",m_transitTimes," << m_transitTimes.x << ',' << m_transitTimes.y;
	ss << ",m_endDelays," << m_endDelays.x << ',' << m_endDelays.y;
	ss << ",m_phase," << m_phase;
}

void OPTranslateLinear::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
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

void OPCubeRollHeight::innerUpdate() {
	double angle = std::fmod(GameTimer::gameTime() + m_phase, m_period) * MF::HALFPI - MF::HALFPI;
	Vec2 rot = Vec2{ -m_cubeHeight * 0.5, m_cubeHeight * 0.5 }.rotate(angle);
	m_updateVal = (rot.y - m_cubeHeight * 0.5);
}

void OPCubeRollHeight::transform(Vec3& p) const
{
	p.y -= m_updateVal;
}

void OPCubeRollHeight::serializeInner(std::stringstream& ss) const
{
	ss << ",m_cubeHeight," << m_cubeHeight;
	ss << ",m_period," << m_period;
	ss << ",m_phase," << m_phase;
}

void OPCubeRollHeight::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
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

void OPRotate::innerUpdate()
{
	Mat3::fromAxisAngle(m_axis, m_rotMat, m_rotations * MF::TAU);
}

void OPRotate::transform(Vec3& p) const
{
	p = m_rotMat * p;
}

void OPRotate::serializeInner(std::stringstream& ss) const
{
	ss << ",m_axis," << m_axis.x << ',' << m_axis.y << ',' << m_axis.z;
	ss << ",m_rotations," << m_rotations;
}

void OPRotate::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
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
void OPRotateContinuous::innerUpdate() {
	Mat3::fromAxisAngle(m_axis, m_updateVal, (GameTimer::gameTime() + (m_phase / m_rotationsPerSecond)) * m_rotationsPerSecond * MF::TAU);
}

void OPRotateContinuous::transform(Vec3& p) const
{
	p = m_updateVal * p;
}

void OPRotateContinuous::serializeInner(std::stringstream& ss) const
{
	ss << ",m_axis," << m_axis.x << ',' << m_axis.y << ',' << m_axis.z;
	ss << ",m_rotationsPerSecond," << m_rotationsPerSecond;
	ss << ",m_phase," << m_phase;
}

void OPRotateContinuous::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
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

void OPRotateScan::innerUpdate() {
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

void OPRotateScan::transform(Vec3& p) const
{
	p = m_updateVal * p;
}

void OPRotateScan::serializeInner(std::stringstream& ss) const
{
	ss << ",m_axis," << m_axis.x << ',' << m_axis.y << ',' << m_axis.z;
	ss << ",m_rotations," << m_rotations;
	ss << ",m_transitTimes," << m_transitTimes.x << ',' << m_transitTimes.y;
	ss << ",m_endDelays," << m_endDelays.x << ',' << m_endDelays.y;
	ss << ",m_phase," << m_phase;
}

void OPRotateScan::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
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

void OPRotateDelay::innerUpdate() {
	double completeCycles{ std::floor((GameTimer::gameTime() + m_phase) / m_cycleTime) };
	double time{ std::fmod(GameTimer::gameTime() + m_phase, m_cycleTime) };
	completeCycles += std::min(m_rotateTime, time) / m_rotateTime;
	Mat3::fromAxisAngle(m_axis, m_updateVal, completeCycles * m_angle);
}

void OPRotateDelay::transform(Vec3& p) const
{
	p = m_updateVal * p;
}

void OPRotateDelay::serializeInner(std::stringstream& ss) const
{
	ss << ",m_axis," << m_axis.x << ',' << m_axis.y << ',' << m_axis.z;
	ss << ",m_rotations," << m_rotations;
	ss << ",m_rotateTime," << m_rotateTime;
	ss << ",m_delayTime," << m_delayTime;
	ss << ",m_phase," << m_phase;
}

void OPRotateDelay::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
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

void OPCubeRoller::innerUpdate() {
	double cycleTime = m_axes.size() * m_quarterTurnTime;
	double time = std::fmod(GameTimer::gameTime() + m_phase, cycleTime);
	size_t index = static_cast<size_t>(time);
	time /= m_quarterTurnTime;
	Mat3::fromAxisAngle(m_axes[index], m_updateVal, MF::HALFPI * time);
}

void OPCubeRoller::transform(Vec3& p) const
{
	p = m_updateVal * p;
}

void OPCubeRoller::serializeInner(std::stringstream& ss) const
{
	ss << ",m_axes," << m_axes.size();
	for (size_t i{}; i < m_axes.size(); ++i) {
		ss << ',' << m_axes[i].x << ',' << m_axes[i].y << ',' << m_axes[i].z;
	}
	ss << ",m_quarterTurnTime," << m_quarterTurnTime;
	ss << ",m_phase," << m_phase;
}

void OPCubeRoller::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
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

void Elongate::innerUpdate() {
	m_halfLen = m_length * 0.5;
	m_halfLenNeg = -m_halfLen;
}

void Elongate::serializeInner(std::stringstream& ss) const
{
	ss << ",m_length," << m_length;
	ss << ",m_offset," << m_offset;
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

void OPElongateX::transform(Vec3& p) const
{
	p.x -= std::clamp(p.x - m_offset, m_halfLenNeg, m_halfLen);
}

void OPElongateY::transform(Vec3& p) const
{
	p.y -= std::clamp(p.y - m_offset, m_halfLenNeg, m_halfLen);
}

void OPElongateZ::transform(Vec3& p) const
{
	p.z -= std::clamp(p.z - m_offset, m_halfLenNeg, m_halfLen);
}

void OPElongateXYZ::innerUpdate()
{
	m_halfLens = m_lengths * 0.5;
	m_halfLenNegs = -m_halfLens;
}

void OPElongateXYZ::transform(Vec3& p) const
{
	p -= (p - m_offsets).clamp(m_halfLenNegs, m_halfLens);
}

void OPElongateXYZ::serializeInner(std::stringstream& ss) const
{
	ss << ",m_lengths," << m_lengths.x << ',' << m_lengths.y << ',' << m_lengths.z;
	ss << ",m_offsets," << m_offsets.x << ',' << m_offsets.y << ',' << m_offsets.z;
}

void OPElongateXYZ::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "lengths") {
		propMod->BindToProp(&m_lengths);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "offsets") {
		propMod->BindToProp(&m_offsets);
		m_propMods.push_back(std::move(propMod));
	}
}
