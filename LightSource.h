#pragma once
#include "Vec3.h"
#include "SDFObject.h"

class LightSource : private SDFObject
{
private:
	double m_luminance;
	Vec3 m_position;
public:
	LightSource(Vec3 position, double luminance = 1.0);
	virtual ~LightSource() = default;
	Vec3 position();
	double luminance();
	double signedDistanceInner(Vec3 p) const override;
};