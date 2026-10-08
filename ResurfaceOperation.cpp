#include "ResurfaceOperation.h"
#include <sstream>

void ResurfaceOperation::bindPropertyModifier(std::unique_ptr<Animator> propMod)
{
	if (propMod->propName() == "magnitude") {
		propMod->BindToProperty(&m_magnitude);
		m_propMods.push_back(std::move(propMod));
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

double OPRoundOff::resurface(double distance)
{
	return distance - m_magnitude;
}

void OPRoundOff::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	std::string propName{ "" };
	ss << "\t// OPRoundOff //\n";
	ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
	ss << "\td" << level << " -= pmFloat0;\n\n";
}

double OPOnion::resurface(double distance) {
	return abs(distance) - m_magnitude;
}

void OPOnion::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	std::string propName{ "" };
	ss << "\t// OPOnion //\n";
	ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "magnitude") ? propName : std::to_string(m_magnitude)) << ";\n";
	ss << "\td" << level << " = abs(d" << level << ") - pmFloat0; \n\n";
}

double OPInsideOut::resurface(double distance) {
	return -distance;
}

void OPInsideOut::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	ss << "\t// OPInsideOut //\n";
	ss << "\td" << level << " = -d" << level << ";\n\n";
}
