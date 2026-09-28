#pragma once
#include <vector>
#include <string>
#include "SDFDefaults.h"
#include "Vec3.h"

class LightSource
{
private:
	double m_luminance;
	Vec3 m_position;
	Color m_color;
public:
	LightSource(Vec3 position = DEF::LSPosition, double luminance = DEF::LSLuminance, Color Color = DEF::LSColor);
	virtual ~LightSource() = default;
	Vec3 position();
	double luminance();
	Color color();
	void serialize(std::vector<std::string>& strVec);
};