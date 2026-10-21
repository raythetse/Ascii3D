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

void OPMirror::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	if (m_mirrorX) p.x = abs(p.x);
	if (m_mirrorY) p.y = abs(p.y);
	if (m_mirrorZ) p.z = abs(p.z);
	if (colorOut) *colorOut = colorIn;
}

std::string OPMirror::getSwizzle() const
{
	std::string swizzle{ "." };
	if (m_mirrorX) swizzle += 'x';
	if (m_mirrorY) swizzle += 'y';
	if (m_mirrorZ) swizzle += 'z';

	return swizzle;
}

void OPMirror::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	ss << "\t// OPMirror //\n";
	ss << "\tp" << level << getSwizzle() << " = abs(p" << level << getSwizzle() << "); \n\n";
}

void OPMirror::innerInnerSerialize(std::stringstream& ss) const
{
	ss << " mirrorX " << m_mirrorX;
	ss << " mirrorY " << m_mirrorY;
	ss << " mirrorZ " << m_mirrorZ;
}

void OPTranslate::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	p -= m_translation;
	if (colorOut) *colorOut = colorIn;
}

void OPTranslate::innerInnerSerialize(std::stringstream& ss) const
{
	ss << " translation " << m_translation;
}

void OPTranslate::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	std::string propName{ "" };
	ss << "\t// OPTranslate //\n";
	ss << "\tpmVec30 = " << (getShaderAnimationCalls(propName = "translation") ? propName : m_translation.getShader()) << ";\n";
	ss << "\tp" << level << " -= pmVec30; \n\n";
}

void OPTranslate::bindAnimation(std::unique_ptr<Animation> animation)
{
	if (animation->propName() == "translation") {
		animation->BindToProperty(&m_translation);
		m_animations.push_back(std::move(animation));
	}
}

void OPRotate::innerUpdate()
{
	Mat3::fromAxisAngle(m_axis, m_rotMat, m_rotations * MF::TAU);
}

void OPRotate::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	p = m_rotMat * p;
	if (colorOut) *colorOut = colorIn;
}

void OPRotate::innerInnerSerialize(std::stringstream& ss) const
{
	ss << " axis " << m_axis;
	ss << " rotations " << m_rotations;
}

void OPRotate::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	std::string propName{ "" };
	ss << "\t// OPRotate //\n";
	ss << "\tpmVec30 = " << (getShaderAnimationCalls(propName = "axis") ? propName : m_axis.getShader()) << ";\n";
	ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "rotations") ? propName : std::to_string(m_rotations)) << ";\n";
	ss << "\tp" << level << " = rotateVec3(p" << level << ", pmVec30, pmFloat0);\n\n";
}

void OPRotate::bindAnimation(std::unique_ptr<Animation> animation)
{
	if (animation->propName() == "axis") {
		animation->BindToProperty(&m_axis);
		m_animations.push_back(std::move(animation));
	}
	else if (animation->propName() == "rotations") {
		animation->BindToProperty(&m_rotations);
		m_animations.push_back(std::move(animation));
	}
	else if (animation->propName() == "rotMat") {
		animation->BindToProperty(&m_rotMat);
		m_animations.push_back(std::move(animation));
	}
}

void OPCubeRollHeight::innerUpdate() {
	double angle = std::fmod(GameTimer::gameTime() + m_phase, m_period) * MF::HALFPI - MF::HALFPI;
	Vec2 rot = Vec2{ -m_cubeHeight * 0.5, m_cubeHeight * 0.5 }.rotate(angle);
	m_updateVal = (rot.y - m_cubeHeight * 0.5);
}

void OPCubeRollHeight::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	p.y -= m_updateVal;
	if (colorOut) *colorOut = colorIn;
}

void OPCubeRollHeight::innerInnerSerialize(std::stringstream& ss) const
{
	ss << " cubeHeight " << m_cubeHeight;
	ss << " period " << m_period;
	ss << " phase " << m_phase;
}

void OPCubeRollHeight::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	std::string propName{ "" };
	ss << "\t// OPCubeRollHeight //\n";
	ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "phase") ? propName : std::to_string(m_phase)) << ";\n";
	ss << "\tpmFloat1 = " << (getShaderAnimationCalls(propName = "period") ? propName : std::to_string(m_period)) << ";\n";
	ss << "\tpmFloat2 = " << (getShaderAnimationCalls(propName = "cubeHeight") ? propName : std::to_string(m_cubeHeight)) << ";\n";
	ss << "\tpmFloat0 = mod(time + pmFloat0, pmFloat1) * HALFPI - HALFPI;\n";
	ss << "\tpmFloat1 = cos(pmFloat0);\n";
	ss << "\tpmFloat0 = sin(pmFloat0);\n";
	ss << "\tpmVec20 = vec2(-pmFloat2 * 0.5, pmFloat2 * 0.5);\n";
	ss << "\tpmVec20 = vec2(pmVec20.x * pmFloat1 - pmVec20.y * pmFloat0, pmVec20.x * pmFloat0 + pmVec20.y * pmFloat1);\n";
	ss << "\tpmFloat0 = (pmVec20.y - pmFloat2 * 0.5);\n";
	ss << "\tp" << level << ".y -= pmFloat0;\n\n";
}

void OPCubeRollHeight::bindAnimation(std::unique_ptr<Animation> animation)
{
	if (animation->propName() == "cubeHeight") {
		animation->BindToProperty(&m_cubeHeight);
		m_animations.push_back(std::move(animation));
	}
	else if (animation->propName() == "period") {
		animation->BindToProperty(&m_period);
		m_animations.push_back(std::move(animation));
	}
	else if (animation->propName() == "phase") {
		animation->BindToProperty(&m_phase);
		m_animations.push_back(std::move(animation));
	}
}

void OPCubeRoller::innerUpdate() {
	double cycleTime = m_axes.size() * m_quarterTurnTime;
	double time = std::fmod(GameTimer::gameTime() + m_phase, cycleTime);
	size_t index = static_cast<size_t>(time);
	time /= m_quarterTurnTime;
	Mat3::fromAxisAngle(m_axes[index], m_updateVal, MF::HALFPI * time);
}

void OPCubeRoller::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	p = m_updateVal * p;
	if (colorOut) *colorOut = colorIn;
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

void OPCubeRoller::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	static size_t numRollers{ 0 };
	std::string propName{ "" };
	ss << "\t// OPCubeRoller //\n";
	ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "phase") ? propName : std::to_string(m_phase)) << ";\n";
	ss << "\tpmFloat1 = " << (getShaderAnimationCalls(propName = "quarterTurnTime") ? propName : std::to_string(m_quarterTurnTime)) << ";\n";
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

void OPCubeRoller::bindAnimation(std::unique_ptr<Animation> animation)
{
	if (animation->propName() == "axes") {
		animation->BindToProperty(&m_axes);
		m_animations.push_back(std::move(animation));
	}
	else if (animation->propName() == "phase") {
		animation->BindToProperty(&m_phase);
		m_animations.push_back(std::move(animation));
	}
	else if (animation->propName() == "quarterTurnTime") {
		animation->BindToProperty(&m_quarterTurnTime);
		m_animations.push_back(std::move(animation));
	}
}

void OPElongate::innerUpdate() {
	m_halfLen = abs(m_length * 0.5);
}

void OPElongate::innerInnerSerialize(std::stringstream& ss) const
{
	ss << " length " << m_length;
	ss << " offset " << m_offset;
	ss << " axis " << m_axis;
}

void OPElongate::bindAnimation(std::unique_ptr<Animation> animation)
{
	if (animation->propName() == "length") {
		animation->BindToProperty(&m_length);
		m_animations.push_back(std::move(animation));
	}
	else if (animation->propName() == "offset") {
		animation->BindToProperty(&m_offset);
		m_animations.push_back(std::move(animation));
	}
}

double OPElongate::executeTerm(double d) const
{
	if (m_length >= 0.0) {
		return d - std::clamp(d - m_offset, -m_halfLen, m_halfLen);
	}
	else {
		return d - (d > m_offset ? -m_halfLen : m_halfLen);
	}
}

void OPElongate::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	switch (m_axis)
	{
	case AXIS_X:
		p.x = executeTerm(p.x);
		break;
	case AXIS_Y:
		p.y = executeTerm(p.y);
		break;
	case AXIS_Z:
		p.z = executeTerm(p.z);
		break;
	default:
		break;
	}
	if (colorOut) *colorOut = colorIn;
}

void OPElongate::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	std::string propName{ "" };
	ss << "\t// OPElongate //\n";
	ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "length") ? propName : std::to_string(m_length)) << ";\n";
	ss << "\tpmFloat1 = " << (getShaderAnimationCalls(propName = "offset") ? propName : std::to_string(m_offset)) << ";\n";
	switch (m_axis)
	{
	case AXIS_X:
		ss << "\tp" << level << ".x = elongate(p" << level << ".x, pmFloat0, pmFloat1);\n\n";
		break;
	case AXIS_Y:
		ss << "\tp" << level << ".y = elongate(p" << level << ".y, pmFloat0, pmFloat1);\n\n";
		break;
	case AXIS_Z:
		ss << "\tp" << level << ".z = elongate(p" << level << ".z, pmFloat0, pmFloat1);\n\n";
		break;
	}
}
