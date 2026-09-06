#pragma once
#include "Vec3.h"
#include "SceneObject.h"

class LightSource : private VisualObject
{
private:
	double m_luminance;
public:
	LightSource(Vec3 position, double luminance = 1.0);
	virtual ~LightSource() = default;
	double luminance();
	using SceneObject::update;
	using SceneObject::position;
	double signedDistanceInner(Vec3 p, double time) const override;
};

