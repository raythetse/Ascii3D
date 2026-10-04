#include "TransformOperation.h"
#include "GameTimer.h"
#include <algorithm>
#include <sstream>

void TransformOperation::innerSerialize(std::vector<std::string>& strVec, size_t level) const
{
	std::stringstream ss{};
	ss << std::string(level, '\t');
	ss << getClassName();
	innerInnerSerialize(ss);
	strVec.push_back(ss.str());
}

void OPMirrorX::transform(Vec3& p) const
{
	p.x = abs(p.x);
}

void OPMirrorX::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	ss << "\t// OPMirrorX //\n";
	ss << "\tp" << level << ".x = abs(p" << level << ".x);\n\n";
}

void OPMirrorY::transform(Vec3& p) const
{
	p.y = abs(p.y);
}

void OPMirrorY::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	ss << "\t// OPMirrorY //\n";
	ss << "\tp" << level << ".y = abs(p" << level << ".y);\n\n";
}

void OPMirrorZ::transform(Vec3& p) const
{
	p.z = abs(p.z);
}

void OPMirrorZ::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	ss << "\t// OPMirrorZ //\n";
	ss << "\tp" << level << ".z = abs(p" << level << ".z);\n\n";
}

void OPMirrorXY::transform(Vec3& p) const
{
	p.x = abs(p.x);
	p.y = abs(p.y);
}

void OPMirrorXY::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	ss << "\t// OPMirrorXY //\n";
	ss << "\tp" << level << ".xy = abs(p" << level << ".xy);\n\n";
}

void OPMirrorXZ::transform(Vec3& p) const
{
	p.x = abs(p.x);
	p.z = abs(p.z);
}

void OPMirrorXZ::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	ss << "\t// OPMirrorXZ //\n";
	ss << "\tp" << level << ".xz = abs(p" << level << ".xz);\n\n";
}

void OPMirrorYZ::transform(Vec3& p) const
{
	p.y = abs(p.y);
	p.z = abs(p.z);
}

void OPMirrorYZ::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	ss << "\t// OPMirrorYZ //\n";
	ss << "\tp" << level << ".yz = abs(p" << level << ".yz);\n\n";
}

void OPMirrorXYZ::transform(Vec3& p) const
{
	p = p.abs();
}

void OPMirrorXYZ::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	ss << "\t// OPMirrorXYZ //\n";
	ss << "\tp" << level << " = abs(p" << level << ");\n\n";
}

void OPTranslate::transform(Vec3& p) const
{
	p -= m_translation;
}

void OPTranslate::innerInnerSerialize(std::stringstream& ss) const
{
	ss << " translation " << m_translation.x << ' ' << m_translation.y << ' ' << m_translation.z;
}

void OPTranslate::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	std::string propName{ "" };
	ss << "\t// OPTranslate //\n";
	ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "translation") ? propName : m_translation.getShader()) << ";\n";
	ss << "\tp" << level << " -= pmVec30; \n\n";
}

void OPTranslate::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "translation") {
		propMod->BindToProp(&m_translation);
		m_propMods.push_back(std::move(propMod));
	}
}

void OPTranslateSine::innerUpdate() {
	m_updateVal = m_axis * m_amplitude * sin(MF::TAU * (GameTimer::gameTime() / m_period + m_phase));
}

void OPTranslateSine::transform(Vec3& p) const
{
	p -= m_updateVal;
}

void OPTranslateSine::innerInnerSerialize(std::stringstream& ss) const
{
	ss << " axis " << m_axis.x << ' ' << m_axis.y << ' ' << m_axis.z;
	ss << " amplitude " << m_amplitude;
	ss << " period " << m_period;
	ss << " phase " << m_phase;
}

void OPTranslateSine::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	std::string propName{ "" };
	ss << "\t// OPTranslateSine //\n";
	ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "amplitude") ? propName : std::to_string(m_amplitude)) << ";\n";
	ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "phase") ? propName : std::to_string(m_phase)) << ";\n";
	ss << "\tpmFloat2 = " << (getShaderPropModCalls(propName = "period") ? propName : std::to_string(m_period)) << ";\n";
	ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "axis") ? propName : m_axis.getShader()) << ";\n";
	ss << "\tp" << level << " -= pmVec30 * pmFloat0 * sin(TAU * (pmFloat1 + time / pmFloat2));\n\n";
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
	double cycleTime = m_transitTimes.x + m_transitTimes.y + m_endDelays.x + m_endDelays.y;
	double progress{ std::fmod(GameTimer::gameTime() + m_phase + m_transitTimes.x * 0.5, cycleTime) };
	Vec3 start{ m_vector * -0.5 };
	if (progress <= m_transitTimes.x) {
		progress /= m_transitTimes.x;
		m_updateVal = start.lerp(-start, progress);
		return;
	}
	double timeFrame = m_transitTimes.x + m_endDelays.x;
	if (progress <= timeFrame) {
		m_updateVal = -start;
		return;
	}
	if (progress <= (timeFrame + m_transitTimes.y)) {
		progress = (progress - m_transitTimes.x - m_endDelays.x) / m_transitTimes.y;
		m_updateVal = (-start).lerp(start, progress);
		return;
	}
	m_updateVal = start;
}

void OPTranslateLinear::transform(Vec3& p) const
{
	p -= m_updateVal;
}

void OPTranslateLinear::innerInnerSerialize(std::stringstream& ss) const
{
	ss << " vector " << m_vector.x << ' ' << m_vector.y << ' ' << m_vector.z;
	ss << " transitTimes " << m_transitTimes.x << ' ' << m_transitTimes.y;
	ss << " endDelays " << m_endDelays.x << ' ' << m_endDelays.y;
	ss << " phase " << m_phase;
}

void OPTranslateLinear::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	std::string propName{ "" };
	ss << "\t// OPTranslateLinear //\n";
	ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "phase") ? propName : std::to_string(m_phase)) << ";\n";
	ss << "\tpmVec20 = " << (getShaderPropModCalls(propName = "transitTimes") ? propName : m_transitTimes.getShader()) << ";\n";
	ss << "\tpmVec21 = " << (getShaderPropModCalls(propName = "endDelays") ? propName : m_endDelays.getShader()) << ";\n";
	ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "vector") ? propName : m_vector.getShader()) << ";\n";
	ss << "\tp" << level << " -= translateLinear(pmVec30, pmVec20, pmVec21, pmFloat0, time);\n\n";
}

void OPTranslateLinear::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "vector") {
		propMod->BindToProp(&m_vector);
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

void OPCubeRollHeight::innerUpdate() {
	double angle = std::fmod(GameTimer::gameTime() + m_phase, m_period) * MF::HALFPI - MF::HALFPI;
	Vec2 rot = Vec2{ -m_cubeHeight * 0.5, m_cubeHeight * 0.5 }.rotate(angle);
	m_updateVal = (rot.y - m_cubeHeight * 0.5);
}

void OPCubeRollHeight::transform(Vec3& p) const
{
	p.y -= m_updateVal;
}

void OPCubeRollHeight::innerInnerSerialize(std::stringstream& ss) const
{
	ss << " cubeHeight " << m_cubeHeight;
	ss << " period " << m_period;
	ss << " phase " << m_phase;
}

void OPCubeRollHeight::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	std::string propName{ "" };
	ss << "\t// OPCubeRollHeight //\n";
	ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "phase") ? propName : std::to_string(m_phase)) << ";\n";
	ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "period") ? propName : std::to_string(m_period)) << ";\n";
	ss << "\tpmFloat2 = " << (getShaderPropModCalls(propName = "cubeHeight") ? propName : std::to_string(m_cubeHeight)) << ";\n";
	ss << "\tpmFloat0 = mod(time + pmFloat0, pmFloat1) * HALFPI - HALFPI;\n";
	ss << "\tpmFloat1 = cos(pmFloat0);\n";
	ss << "\tpmFloat0 = sin(pmFloat0);\n";
	ss << "\tpmVec20 = vec2(-pmFloat2 * 0.5, pmFloat2 * 0.5);\n";
	ss << "\tpmVec20 = vec2(pmVec20.x * pmFloat1 - pmVec20.y * pmFloat0, pmVec20.x * pmFloat0 + pmVec20.y * pmFloat1);\n";
	ss << "\tpmFloat0 = (pmVec20.y - pmFloat2 * 0.5);\n";
	ss << "\tp" << level << ".y -= pmFloat0;\n\n";
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

void OPRotate::innerInnerSerialize(std::stringstream& ss) const
{
	ss << " axis " << m_axis.x << ' ' << m_axis.y << ' ' << m_axis.z;
	ss << " rotations " << m_rotations;
}

void OPRotate::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	std::string propName{ "" };
	ss << "\t// OPRotate //\n";
	ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "axis") ? propName : m_axis.getShader()) << ";\n";
	ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "rotations") ? propName : std::to_string(m_rotations)) << ";\n";
	ss << "\tp" << level << " = rotateVec3(p" << level << ", pmVec30, pmFloat0);\n\n";
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

void OPRotateContinuous::innerInnerSerialize(std::stringstream& ss) const
{
	ss << " axis " << m_axis.x << ' ' << m_axis.y << ' ' << m_axis.z;
	ss << " rotationsPerSecond " << m_rotationsPerSecond;
	ss << " phase " << m_phase;
}

void OPRotateContinuous::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	std::string propName{ "" };
	ss << "\t// OPRotateContinuous //\n";
	ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "axis") ? propName : m_axis.getShader()) << ";\n";
	ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "rotationsPerSecond") ? propName : std::to_string(m_rotationsPerSecond)) << ";\n";
	ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "phase") ? propName : std::to_string(m_phase)) << ";\n";
	ss << "\tp" << level << " = rotateVec3(p" << level << ", pmVec30, time * pmFloat0 + pmFloat1);\n\n";
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
	double progress{ std::fmod(GameTimer::gameTime() + m_phase, m_transitTimes.x + m_transitTimes.y + m_endDelays.x + m_endDelays.y) };
	double endAngle = m_rotations * MF::TAU;
	if (progress <= m_transitTimes.x) {
		progress /= m_transitTimes.x;
		Mat3::fromAxisAngle(m_axis, m_updateVal, endAngle * progress);
		return;
	}
	double timeFrame = m_transitTimes.x + m_endDelays.x;
	if (progress <= timeFrame) {
		Mat3::fromAxisAngle(m_axis, m_updateVal, endAngle);
		return;
	}
	if (progress <= timeFrame + m_transitTimes.y) {
		progress = (progress - (m_transitTimes.x + m_endDelays.x)) / m_transitTimes.y;
		Mat3::fromAxisAngle(m_axis, m_updateVal, endAngle * (1.0 - progress));
		return;
	}
	m_updateVal = Mat3::Identity;
}

void OPRotateScan::transform(Vec3& p) const
{
	p = m_updateVal * p;
}

void OPRotateScan::innerInnerSerialize(std::stringstream& ss) const
{
	ss << " axis " << m_axis.x << ' ' << m_axis.y << ' ' << m_axis.z;
	ss << " rotations " << m_rotations;
	ss << " transitTimes " << m_transitTimes.x << ' ' << m_transitTimes.y;
	ss << " endDelays " << m_endDelays.x << ' ' << m_endDelays.y;
	ss << " phase " << m_phase;
}

void OPRotateScan::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	std::string propName{ "" };
	ss << "\t// OPRotateScan //\n";
	ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "axis") ? propName : m_axis.getShader()) << ";\n";
	ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "rotations") ? propName : std::to_string(m_rotations)) << ";\n";
	ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "phase") ? propName : std::to_string(m_phase)) << ";\n";
	ss << "\tpmVec20 = " << (getShaderPropModCalls(propName = "transitTimes") ? propName : m_transitTimes.getShader()) << ";\n";
	ss << "\tpmVec21 = " << (getShaderPropModCalls(propName = "endDelays") ? propName : m_endDelays.getShader()) << ";\n";
	ss << "\tp" << level << " = rotateScan(p" << level << ", pmVec30, pmVec20, pmVec21, pmFloat0, pmFloat1, time);\n\n";
}

void OPRotateScan::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "axis") {
		propMod->BindToProp(&m_axis);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "rotations") {
		propMod->BindToProp(&m_rotations);
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
	double cycleTime = m_rotateTime + m_delayTime;
	double completeCycles{ std::floor((GameTimer::gameTime() + m_phase) / cycleTime) };
	double progress{ std::fmod(GameTimer::gameTime() + m_phase, cycleTime) };
	completeCycles += std::min(m_rotateTime, progress) / m_rotateTime;
	Mat3::fromAxisAngle(m_axis, m_updateVal, completeCycles * m_rotations * MF::TAU);
}

void OPRotateDelay::transform(Vec3& p) const
{
	p = m_updateVal * p;
}

void OPRotateDelay::innerInnerSerialize(std::stringstream& ss) const
{
	ss << " axis " << m_axis.x << ' ' << m_axis.y << ' ' << m_axis.z;
	ss << " rotations " << m_rotations;
	ss << " rotateTime " << m_rotateTime;
	ss << " delayTime " << m_delayTime;
	ss << " phase " << m_phase;
}

void OPRotateDelay::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	std::string propName{ "" };
	ss << "\t// OPRotateDelay //\n";
	ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "axis") ? propName : m_axis.getShader()) << ";\n";
	ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "rotations") ? propName : std::to_string(m_rotations)) << ";\n";
	ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "rotateTime") ? propName : std::to_string(m_rotateTime)) << ";\n";
	ss << "\tpmFloat2 = " << (getShaderPropModCalls(propName = "delayTime") ? propName : std::to_string(m_delayTime)) << ";\n";
	ss << "\tpmVec31.x = " << (getShaderPropModCalls(propName = "phase") ? propName : std::to_string(m_phase)) << ";\n";
	ss << "\tp" << level << " = rotateDelay(p" << level << ", pmVec30, pmFloat0, pmFloat1, pmFloat2, pmVec31.x, time);\n\n";
}

void OPRotateDelay::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "axis") {
		propMod->BindToProp(&m_axis);
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
	else if (propMod->propName() == "rotations") {
		propMod->BindToProp(&m_rotations);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "delayTime") {
		propMod->BindToProp(&m_delayTime);
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

void OPCubeRoller::innerInnerSerialize(std::stringstream& ss) const
{
	ss << " axes " << m_axes.size();
	for (size_t i{}; i < m_axes.size(); ++i) {
		ss << ' ' << m_axes[i].x << ' ' << m_axes[i].y << ' ' << m_axes[i].z;
	}
	ss << " quarterTurnTime " << m_quarterTurnTime;
	ss << " phase " << m_phase;
}

void OPCubeRoller::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	static size_t numRollers{ 0 };
	std::string propName{ "" };
	ss << "\t// OPCubeRoller //\n";
	ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "phase") ? propName : std::to_string(m_phase)) << ";\n";
	ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "quarterTurnTime") ? propName : std::to_string(m_quarterTurnTime)) << ";\n";
	ss << "\tvec3 axes" << numRollers << "[" << m_axes.size() << "]; \n";
	for (size_t i{}; i < m_axes.size(); ++i) {
		ss << "\taxes" << numRollers << "[" << i << "] = " << m_axes[i].getShader() << ";\n";
	}
	ss << "\tpmFloat2 = " << m_axes.size() << " * pmFloat1;\n";
	ss << "\tpmFloat2 = mod(time + pmFloat0, pmFloat2);\n";
	ss << "\tpmFloat0 = floor(pmFloat2);\n";
	ss << "\tpmFloat2 /= pmFloat1;\n";
	ss << "\tp" << level << " = rotateVec3(p" << level << ", axes" << numRollers << "[int(pmFloat0)], 0.25 * pmFloat2);\n\n";

	++numRollers;
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

void Elongate::innerInnerSerialize(std::stringstream& ss) const
{
	ss << " length " << m_length;
	ss << " offset " << m_offset;
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

void OPElongateX::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	std::string propName{ "" };
	ss << "\t// OPElongateX //\n";
	ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "length") ? propName : std::to_string(m_length)) << ";\n";
	ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "offset") ? propName : std::to_string(m_offset)) << ";\n";
	ss << "\tp" << level << ".x -= clamp(p" << level << ".x - pmFloat1, -0.5 * pmFloat0, 0.5 * pmFloat0);\n\n";
}

void OPElongateY::transform(Vec3& p) const
{
	p.y -= std::clamp(p.y - m_offset, m_halfLenNeg, m_halfLen);
}

void OPElongateY::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	std::string propName{ "" };
	ss << "\t// OPElongateY //\n";
	ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "length") ? propName : std::to_string(m_length)) << ";\n";
	ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "offset") ? propName : std::to_string(m_offset)) << ";\n";
	ss << "\tp" << level << ".y -= clamp(p" << level << ".y - pmFloat1, -0.5 * pmFloat0, 0.5 * pmFloat0);\n\n";
}

void OPElongateZ::transform(Vec3& p) const
{
	p.z -= std::clamp(p.z - m_offset, m_halfLenNeg, m_halfLen);
}

void OPElongateZ::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	std::string propName{ "" };
	ss << "\t// OPElongateZ //\n";
	ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "length") ? propName : std::to_string(m_length)) << ";\n";
	ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "offset") ? propName : std::to_string(m_offset)) << ";\n";
	ss << "\tp" << level << ".z -= clamp(p" << level << ".z - pmFloat1, -0.5 * pmFloat0, 0.5 * pmFloat0);\n\n";
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

void OPElongateXYZ::innerInnerSerialize(std::stringstream& ss) const
{
	ss << " lengths " << m_lengths.x << ' ' << m_lengths.y << ' ' << m_lengths.z;
	ss << " offsets " << m_offsets.x << ' ' << m_offsets.y << ' ' << m_offsets.z;
}

void OPElongateXYZ::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	std::string propName{ "" };
	ss << "\t// OPElongateXYZ //\n";
	ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "lengths") ? propName : m_lengths.getShader()) << ";\n";
	ss << "\tpmVec31 = " << (getShaderPropModCalls(propName = "offsets") ? propName : m_offsets.getShader()) << ";\n";
	ss << "\tp" << level << " -= clamp(p" << level << " - pmVec31, -0.5 * pmVec30, 0.5 * pmVec30);\n\n";
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
