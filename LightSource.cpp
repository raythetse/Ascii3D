#include "LightSource.h"
#include <sstream>

LightSource::LightSource(Vec3 position, double luminance, Color color, double ambientPower)
	: m_color(color), m_luminance(luminance), m_position(position), m_ambientPower(ambientPower) {}

Vec3 LightSource::position()
{
	return m_position;
}

double LightSource::luminance()
{
	return m_luminance;
}

double LightSource::ambientPower()
{
	return m_ambientPower;
}

Color LightSource::color()
{
	return m_color;
}

void LightSource::serialize(std::vector<std::string>& strVec)
{
	std::stringstream ss{};
	ss << "LightSource";
	ss << " m_position " << m_position.x << ' ' << m_position.y << ' ' << m_position.z;
	ss << " m_luminance " << m_luminance;
	ss << " m_color " << m_color.x << ' ' << m_color.y << ' ' << m_color.z;
	ss << " m_ambientPower " << m_ambientPower;
	strVec.push_back(ss.str());
}
