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
	ss << " translation " << m_translation;
}

void OPTranslate::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	std::string propName{ "" };
	ss << "\t// OPTranslate //\n";
	ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "translation") ? propName : m_translation.getShader()) << ";\n";
	ss << "\tp" << level << " -= pmVec30; \n\n";
}

void OPTranslate::bindPropertyModifier(std::unique_ptr<Animator> propMod)
{
	if (propMod->propName() == "translation") {
		propMod->BindToProperty(&m_translation);
		m_animations.push_back(std::move(propMod));
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
	ss << " axis " << m_axis;
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

void OPRotate::bindPropertyModifier(std::unique_ptr<Animator> propMod)
{
	if (propMod->propName() == "axis") {
		propMod->BindToProperty(&m_axis);
		m_animations.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "rotations") {
		propMod->BindToProperty(&m_rotations);
		m_animations.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "rotMat") {
		propMod->BindToProperty(&m_rotMat);
		m_animations.push_back(std::move(propMod));
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

void OPCubeRollHeight::bindPropertyModifier(std::unique_ptr<Animator> propMod)
{
	if (propMod->propName() == "cubeHeight") {
		propMod->BindToProperty(&m_cubeHeight);
		m_animations.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "period") {
		propMod->BindToProperty(&m_period);
		m_animations.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "phase") {
		propMod->BindToProperty(&m_phase);
		m_animations.push_back(std::move(propMod));
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
		ss << ' ' << m_axes[i];
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

void OPCubeRoller::bindPropertyModifier(std::unique_ptr<Animator> propMod)
{
	if (propMod->propName() == "axes") {
		propMod->BindToProperty(&m_axes);
		m_animations.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "phase") {
		propMod->BindToProperty(&m_phase);
		m_animations.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "quarterTurnTime") {
		propMod->BindToProperty(&m_quarterTurnTime);
		m_animations.push_back(std::move(propMod));
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

void Elongate::bindPropertyModifier(std::unique_ptr<Animator> propMod)
{
	if (propMod->propName() == "length") {
		propMod->BindToProperty(&m_length);
		m_animations.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "offset") {
		propMod->BindToProperty(&m_offset);
		m_animations.push_back(std::move(propMod));
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
	ss << " lengths " << m_lengths;
	ss << " offsets " << m_offsets;
}

void OPElongateXYZ::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	std::string propName{ "" };
	ss << "\t// OPElongateXYZ //\n";
	ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "lengths") ? propName : m_lengths.getShader()) << ";\n";
	ss << "\tpmVec31 = " << (getShaderPropModCalls(propName = "offsets") ? propName : m_offsets.getShader()) << ";\n";
	ss << "\tp" << level << " -= clamp(p" << level << " - pmVec31, -0.5 * pmVec30, 0.5 * pmVec30);\n\n";
}

void OPElongateXYZ::bindPropertyModifier(std::unique_ptr<Animator> propMod)
{
	if (propMod->propName() == "lengths") {
		propMod->BindToProperty(&m_lengths);
		m_animations.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "offsets") {
		propMod->BindToProperty(&m_offsets);
		m_animations.push_back(std::move(propMod));
	}
}
