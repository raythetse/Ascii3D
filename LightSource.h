#pragma once
#include <vector>
#include <string>
#include "SDFDefaults.h"
#include "Vec3.h"
#include "SDFObject.h"

class LightSource : private SDFObject
{
private:
	double m_luminance;
	Vec3 m_position;
public:
	LightSource(Vec3 position = DEF::LSPosition, double luminance = DEF::LSLuminance, Color Color = DEF::LSColor);
	virtual ~LightSource() = default;
	Vec3 position();
	double luminance();
	Color color();
	double signedDistanceInner(Vec3 p) const override;
	void serialize(std::vector<std::string>& strVec);
};