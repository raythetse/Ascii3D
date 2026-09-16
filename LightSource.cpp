#include "LightSource.h"
#include <sstream>

LightSource::LightSource(Vec3 position, double luminance)
	: SDFObject(Color::White, false), m_luminance(1000.0 * luminance), m_position(position) {}

Vec3 LightSource::position()
{
	return m_position;
}

double LightSource::luminance()
{
	return m_luminance;
}

double LightSource::signedDistanceInner(Vec3 p) const
{
	return 0.0;
}

void LightSource::serialize(std::vector<std::string>& strVec)
{
	std::stringstream ss{};
	ss << "LightSource";
	ss << " m_position " << m_position.x << ' ' << m_position.y << ' ' << m_position.z;
	ss << " m_luminance " << ' ' << (m_luminance / 1000.0);
	strVec.push_back(ss.str());
}
