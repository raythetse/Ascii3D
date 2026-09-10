#include "ResurfaceOperation.h"

void* ResurfaceOperation::getPropertyPtr(std::string_view propName)
{
	if (propName == "magnitude") return &m_magnitude;
	return nullptr;
}

double RoundOff::resurface(double distance)
{
	return distance - m_magnitude;
}

double Onion::resurface(double distance) {
	return abs(distance) - m_magnitude;
}