#include "CombinationOperation.h"
#include <algorithm>
#include <sstream>

void CombinationOperation::blendColors(double alpha, Vec3 colorIn, Vec3* colorOut)
{
	alpha *= m_colorBlending;
	alpha -= 0.5 * (m_colorBlending - 1.0);
	alpha = alpha < 0.0 ? 0.0 : alpha;
	alpha = alpha > 1.0 ? 1.0 : alpha;
	*colorOut = colorIn.lerp(*colorOut, alpha);
}

void CombinationOperation::setCombineObject(std::unique_ptr<SDFObject> combineObj)
{
	m_combineObj = std::move(combineObj);
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

void CombinationOperation::serialize(std::vector<std::string>& strVec, size_t level) const
{
	for (auto& propMod : m_propMods) {
		propMod->resetProperty();
	}
	innerSerialize(strVec, level);
	for (auto& propMod : m_propMods) {
		propMod->serialize(strVec, level);
	}
	m_combineObj->serialize(strVec, level);
}

void CombinationOperation::innerSerialize(std::vector<std::string>& strVec, size_t level) const
{
	std::stringstream ss{};
	ss << std::string(level, '\t');
	ss << getClassName();
	ss << " m_smoothing " << m_smoothing;
	ss << " m_colorBlending " << m_colorBlending;
	ss << " m_colorShifting " << m_colorShifting;
	strVec.push_back(ss.str());
}

double COMBAdd::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	if (distanceA <= distanceB) {
		if (colorOut) *colorOut = colorIn;
		return distanceA;
	}
	return distanceB;
}

double COMBSubtract::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = -m_combineObj->signedDistance(p, colorOut);
	if (colorOut) *colorOut = colorIn;
	return distanceA > distanceB ? distanceA : distanceB;
}

double COMBIntersect::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	if (distanceA <= distanceB) {
		if (colorOut) *colorOut = colorIn;
		return distanceB;
	}
	return distanceA;
}

double COMBAddSmooth::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	double h = std::clamp(0.5 + 0.5 * (distanceB - distanceA) / m_smoothing, 0.0, 1.0);
	if (colorOut) blendColors(std::clamp(1.0 - h + m_colorShifting, 0.0, 1.0), colorIn, colorOut);
	return std::lerp(distanceB, distanceA, h) - m_smoothing * h * (1.0 - h);
}

double COMBSubtractSmooth::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	double h = m_smoothing - abs(-distanceB - distanceA);
	h = h < 0.0 ? 0.0 : h;
	if (colorOut) blendColors(std::clamp(h + m_colorShifting, 0.0, 1.0), colorIn, colorOut);
	return (distanceA > -distanceB ? distanceA : -distanceB) + h * h * 0.25 / m_smoothing;
}

double COMBIntersectSmooth::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	double h = std::clamp(0.5 - 0.5 * (distanceB - distanceA) / m_smoothing, 0.0, 1.0);
	if (colorOut) blendColors(std::clamp(h + m_colorShifting, 0.0, 1.0), colorIn, colorOut);
	return std::lerp(distanceB, distanceA, h) + m_smoothing * h * (1.0 - h);
}