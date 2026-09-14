#include "ResurfaceOperation.h"
#include <sstream>

void ResurfaceOperation::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "magnitude") {
		propMod->BindToProp(&m_magnitude);
		m_propMods.push_back(std::move(propMod));
	}
}

void ResurfaceOperation::serialize(std::vector<std::string>& strVec, size_t level) const
{
	std::stringstream ss{};
	ss << std::string(level, '\t');
	ss << getClassName();
	ss << " m_magnitude " << m_magnitude;
	ss << " m_updateValue " << m_updateValue;

	strVec.push_back(ss.str());
}

double OPRoundOff::resurface(double distance)
{
	return distance - m_magnitude;
}

double OPOnion::resurface(double distance) {
	return abs(distance) - m_magnitude;
}