#pragma once
#include <vector>
#include <string>
#include "SDFDefaults.h"
#include "Vec3.h"

class LightSource
{
private:
	double m_luminance;
	double m_ambientPower;
	Vec3 m_position;
	Color m_color;
public:
	LightSource(Vec3 position = DEF::LSPosition, double luminance = DEF::LSLuminance, Color color = DEF::LSColor, double ambientPower = DEF::LSAmbientPower);
	virtual ~LightSource() = default;
	Vec3 position();
	double luminance();
	double ambientPower();
	Color color();
	void serialize(std::vector<std::string>& strVec);
};