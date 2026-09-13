#include "CombinationOperation.h"
#include <algorithm>

void CombinationOperation::blendColors(double alpha, Vec3 colorIn, Vec3* colorOut)
{
	alpha *= m_colorBlending;
	alpha -= 0.5 * (m_colorBlending - 1.0);
	alpha = alpha < 0.0 ? 0.0 : alpha;
	alpha = alpha > 1.0 ? 1.0 : alpha;
	*colorOut = colorIn.lerp(*colorOut, alpha);
}

void CombinationOperation::update()
{
	for (auto& propMod : m_propMods) {
		propMod->update();
	}
	innerUpdate();
	m_combineObj->update();
}

void CombinationOperation::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "smoothing") { 
		propMod->BindToProp(&m_smoothing);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "colorBlending") {
		propMod->BindToProp(&m_colorBlending);
		m_propMods.push_back(std::move(propMod));
	}
	else if (propMod->propName() == "colorshifting") {
		propMod->BindToProp(&m_colorShifting);
		m_propMods.push_back(std::move(propMod));
	}
}

double OPAdd::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	if (distanceA <= distanceB) {
		if (colorOut) *colorOut = colorIn;
		return distanceA;
	}
	return distanceB;
}

double OPSubtract::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = -m_combineObj->signedDistance(p, colorOut);
	if (colorOut) *colorOut = colorIn;
	return distanceA > distanceB ? distanceA : distanceB;
}

double OPIntersect::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	if (distanceA <= distanceB) {
		if (colorOut) *colorOut = colorIn;
		return distanceB;
	}
	return distanceA;
}

double OPAddSmooth::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	double h = std::clamp(0.5 + 0.5 * (distanceB - distanceA) / m_smoothing, 0.0, 1.0);
	if (colorOut) blendColors(std::clamp(1.0 - h + m_colorShifting, 0.0, 1.0), colorIn, colorOut);
	return std::lerp(distanceB, distanceA, h) - m_smoothing * h * (1.0 - h);
}

double OPSubtractSmooth::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	double h = m_smoothing - abs(-distanceB - distanceA);
	h = h < 0.0 ? 0.0 : h;
	if (colorOut) blendColors(std::clamp(h + m_colorShifting, 0.0, 1.0), colorIn, colorOut);
	return (distanceA > -distanceB ? distanceA : -distanceB) + h * h * 0.25 / m_smoothing;
}

double OPIntersectSmooth::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	double h = std::clamp(0.5 - 0.5 * (distanceB - distanceA) / m_smoothing, 0.0, 1.0);
	if (colorOut) blendColors(std::clamp(h + m_colorShifting, 0.0, 1.0), colorIn, colorOut);
	return std::lerp(distanceB, distanceA, h) + m_smoothing * h * (1.0 - h);
}