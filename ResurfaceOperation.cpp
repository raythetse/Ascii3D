#include "ResurfaceOperation.h"
#include <sstream>

void ResurfaceOperation::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "magnitude") {
		propMod->BindToProp(&m_magnitude);
		m_propMods.push_back(std::move(propMod));
	}
}

void ResurfaceOperation::innerSerialize(std::vector<std::string>& strVec, size_t level) const
{
	std::stringstream ss{};
	ss << std::string(level, '\t');
	ss << getClassName();
	ss << " m_magnitude " << m_magnitude;

	strVec.push_back(ss.str());
}

double OPRoundOff::resurface(double distance)
{
	return distance - m_magnitude;
}

void OPRoundOff::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	ss << "\t\t\t// OPRoundOff //\n";
}

double OPOnion::resurface(double distance) {
	return abs(distance) - m_magnitude;
}

void OPOnion::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	ss << "\t\t\t// OPOnion //\n";
}

double OPInsideOut::resurface(double distance) {
	return -distance;
}

void OPInsideOut::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	ss << "\t\t\t// OPInsideOut //\n";
	ss << "\t\t\td" << level << " = -d" << level << ";\n";
}
