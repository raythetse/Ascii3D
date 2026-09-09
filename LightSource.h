#pragma once
#include "Vec3.h"
#include "SDFObject.h"

class LightSource : private SDFObject
{
private:
	double m_luminance;
public:
	LightSource(Vec3 position, double luminance = 1.0);
	virtual ~LightSource() = default;
	double luminance();
	using SDFObject::position;
	double signedDistanceInner(Vec3 p) const override;
};