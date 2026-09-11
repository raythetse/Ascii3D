#include "LightSource.h"

LightSource::LightSource(Vec3 position, double luminance)
	: SDFObject(position, Color::White, false), m_luminance(1000.0 * luminance) {}

double LightSource::luminance()
{
	return m_luminance;
}

double LightSource::signedDistanceInner(Vec3 p) const
{
	return 0.0;
}