#include "ResurfaceOperation.h"

void ResurfaceOperation::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "magnitude") {
		propMod->BindToProp(&m_magnitude);
		m_propMods.push_back(std::move(propMod));
	}
}

double RoundOff::resurface(double distance)
{
	return distance - m_magnitude;
}

double Onion::resurface(double distance) {
	return abs(distance) - m_magnitude;
}