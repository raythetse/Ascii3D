#include "LightSource.h"

LightSource::LightSource(Vec3 position, double luminance)
	: VisualObject(position, Vec3::Zero, Color::White, 0.0, false), m_luminance(1000.0 * luminance) {}

double LightSource::luminance()
{
	return m_luminance;
}

double LightSource::signedDistanceInner(Vec3 p, double time) const
{
	return 0.0;
}
