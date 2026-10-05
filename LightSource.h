#pragma once
#include <vector>
#include <string>
#include "Vec3.h"

class LightSource
{
public:
	inline static const Vec3 DFLT_POSITION{ 5.0, 5.0, -3.0 };
	inline static const double DFLT_LUMINANCE{ 100.0 };
	inline static const Color DFLT_COLOR{ 1.0, 1.0, 1.0 };
	inline static const double DFLT_AMBIENTPOWER{ 0.2 };
private:
	double m_luminance;
	double m_ambientPower;
	Vec3 m_position;
	Color m_color;
public:
	LightSource(Vec3 position = DFLT_POSITION, double luminance = DFLT_LUMINANCE, Color color = DFLT_COLOR, double ambientPower = DFLT_AMBIENTPOWER);
	virtual ~LightSource() = default;
	Vec3 position();
	double luminance();
	double ambientPower();
	Color color();
	void serialize(std::vector<std::string>& strVec);
};