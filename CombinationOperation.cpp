#include "CombinationOperation.h"
#include <algorithm>
#include <sstream>

void CombinationOperation::blendColors(double alpha, Vec3 colorIn, Vec3* colorOut)
{
	alpha *= m_colorBlending;
	alpha -= 0.5 * (m_colorBlending - 1.0);
	alpha = std::clamp(alpha, 0.0, 1.0);
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
	else if (propMod->propName() == "colorShifting") {
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

void CombinationOperation::getShaderCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	m_combineObj->getShaderCalls(ss, level + 1, maxLevel);
	getShaderOpCalls(ss, level, maxLevel);
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

void COMBAdd::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	ss << "\t\t\t// COMBAdd //\n";
	ss << "\t\t\tif (d" << (level + 1) << " < d" << level << ") {\n";
	ss << "\t\t\t\td" << level << " = d" << (level + 1) << ";\n";
	ss << "\t\t\t\tcolor" << level << " = color" << (level + 1) << "; \n";
	ss << "\t\t\t}\n\n";
}

double COMBSubtract::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = -m_combineObj->signedDistance(p, colorOut);
	if (colorOut) *colorOut = colorIn;
	return distanceA > distanceB ? distanceA : distanceB;
}

void COMBSubtract::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	ss << "\t\t\t// COMBSubtract //\n";
	ss << "\t\t\tif (d" << level << " < -d" << (level + 1) << ") d" << level << " = -d" << (level + 1) << ";\n\n";
}

double COMBIntersect::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	if (distanceA <= distanceB) {
		if (colorOut) *colorOut = colorIn;
		return distanceB;
	}
	return distanceA;
}

void COMBIntersect::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	ss << "\t\t\t// COMBIntersect //\n";
	ss << "\t\t\tif (d" << level << " <= d" << (level + 1) << ") d" << level << " = d" << (level + 1) << ";\n\n";
}

double COMBAddSmooth::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	double h = std::clamp(0.5 + 0.5 * (distanceB - distanceA) / m_smoothing, 0.0, 1.0);
	if (colorOut) blendColors(std::clamp(1.0 - h + m_colorShifting, 0.0, 1.0), colorIn, colorOut);
	return std::lerp(distanceB, distanceA, h) - m_smoothing * h * (1.0 - h);
}

void COMBAddSmooth::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	ss << "\t\t\t// COMBAddSmooth //\n";
	std::string propName{""};
	ss << "\t\t\tpmFloat0 = " << (getShaderPropModCalls(propName = "smoothing") ? propName : std::to_string(m_smoothing)) << ";\n";
	ss << "\t\t\tpmFloat1 = " << (getShaderPropModCalls(propName = "colorShifting") ? propName : std::to_string(m_colorShifting)) << ";\n";
	ss << "\t\t\tpmFloat2 = " << (getShaderPropModCalls(propName = "colorBlending") ? propName : std::to_string(m_colorBlending)) << ";\n";
	ss << "\t\t\tblend = clamp(0.5 + 0.5 * (d" << (level + 1) << " - d" << level << ") / pmFloat0, 0.0, 1.0);\n";
	ss << "\t\t\tcolor" << level << " = blendColors(clamp(1.0 - blend + pmFloat1, 0.0, 1.0), color" << level << ", color" << (level + 1) << ", pmFloat2);\n";
	ss << "\t\t\td" << level << " = mix(d" << (level + 1) << ", d" << level << ", blend) - pmFloat0 * blend * (1.0 - blend);\n\n";
}

double COMBSubtractSmooth::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	double h = m_smoothing - abs(-distanceB - distanceA);
	h = h < 0.0 ? 0.0 : h;
	if (colorOut) blendColors(std::clamp(h + m_colorShifting, 0.0, 1.0), colorIn, colorOut);
	return (distanceA > -distanceB ? distanceA : -distanceB) + h * h * 0.25 / m_smoothing;
}

void COMBSubtractSmooth::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	ss << "\t\t\t// COMBSubtractSmooth //\n";
	std::string propName{ "" };
	ss << "\t\t\tpmFloat0 = " << (getShaderPropModCalls(propName = "smoothing") ? propName : std::to_string(m_smoothing)) << ";\n";
	ss << "\t\t\tpmFloat1 = " << (getShaderPropModCalls(propName = "colorShifting") ? propName : std::to_string(m_colorShifting)) << ";\n";
	ss << "\t\t\tpmFloat2 = " << (getShaderPropModCalls(propName = "colorBlending") ? propName : std::to_string(m_colorBlending)) << ";\n";
	ss << "\t\t\tblend = pmFloat0 - abs(-d" << (level + 1) << " - d" << level << ");\n";
	ss << "\t\t\tblend = max(blend, 0.0);\n";
	ss << "\t\t\tcolor" << level << " = blendColors(clamp(blend + pmFloat1, 0.0, 1.0), color" << level << ", color" << (level + 1) << ", pmFloat2);\n";
	ss << "\t\t\td" << level << " = (d" << level << " > -d" << (level + 1) << " ? d" << level << " : -d" << (level + 1) << ") + blend * blend * 0.25 / pmFloat0;\n\n";
}

double COMBIntersectSmooth::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	double h = std::clamp(0.5 - 0.5 * (distanceB - distanceA) / m_smoothing, 0.0, 1.0);
	if (colorOut) blendColors(std::clamp(h + m_colorShifting, 0.0, 1.0), colorIn, colorOut);
	return std::lerp(distanceB, distanceA, h) + m_smoothing * h * (1.0 - h);
}

void COMBIntersectSmooth::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	ss << "\t\t\t// COMBIntersectSmooth //\n";
	std::string propName{ "" };
	ss << "\t\t\tpmFloat0 = " << (getShaderPropModCalls(propName = "smoothing") ? propName : std::to_string(m_smoothing)) << ";\n";
	ss << "\t\t\tpmFloat1 = " << (getShaderPropModCalls(propName = "colorShifting") ? propName : std::to_string(m_colorShifting)) << ";\n";
	ss << "\t\t\tpmFloat2 = " << (getShaderPropModCalls(propName = "colorBlending") ? propName : std::to_string(m_colorBlending)) << ";\n";
	ss << "\t\t\tblend = clamp(0.5 - 0.5 * (d" << (level + 1) << " - d" << level << ") / pmFloat0, 0.0, 1.0);\n";
	ss << "\t\t\tcolor" << level << " = blendColors(clamp(blend + pmFloat1, 0.0, 1.0), color" << level << ", color" << (level + 1) << ", pmFloat2);\n";
	ss << "\t\t\td" << level << " = mix(d" << (level + 1) << ", d" << level << ", blend) + pmFloat0 * blend * (1.0 - blend);\n\n";
}
