#include "ResurfaceOperation.h"
#include <sstream>

void ResurfaceOperation::bindAnimation(std::unique_ptr<Animation> animation)
{
	if (animation->propName() == "magnitude") {
		animation->BindToProperty(&m_magnitude);
		m_animations.push_back(std::move(animation));
	}
}

void ResurfaceOperation::innerSerialize(std::vector<std::string>& strVec, size_t level) const
{
	std::stringstream ss{};
	ss << std::string(level, '\t');
	ss << getClassName();
	ss << " magnitude " << m_magnitude;

	strVec.push_back(ss.str());
}

void OPRoundOff::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	distanceA -= m_magnitude;
	if (colorOut) *colorOut = colorIn;
}

void OPRoundOff::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	std::string propName{ "" };
	ss << "\t// OPRoundOff //\n";
	ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
	ss << "\td" << level << " -= pmFloat0;\n\n";
}

void OPOnion::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	distanceA = abs(distanceA) - m_magnitude;
	if (colorOut) *colorOut = colorIn;
}

void OPOnion::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	std::string propName{ "" };
	ss << "\t// OPOnion //\n";
	ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
	ss << "\td" << level << " = abs(d" << level << ") - pmFloat0; \n\n";
}

void OPInsideOut::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	distanceA = -distanceA;
	if (colorOut) *colorOut = colorIn;
}

void OPInsideOut::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	ss << "\t// OPInsideOut //\n";
	ss << "\td" << level << " = -d" << level << ";\n\n";
}
