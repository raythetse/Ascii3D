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
	LightSource(Vec3 position = DEF::LSPosition, double luminance = DEF::LSLuminance);
	virtual ~LightSource() = default;
	Vec3 position();
	double luminance();
	double signedDistanceInner(Vec3 p) const override;
	void serialize(std::vector<std::string>& strVec);
};