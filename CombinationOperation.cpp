#include "CombinationOperation.h"
#include <algorithm>
#include <sstream>

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

void CombinationOperation::serialize(std::vector<std::string>& strVec, size_t level) const
{
	for (auto& propMod : m_propMods) {
		propMod->resetProperty();
	}
	innerSerialize(strVec, level);
	for (auto& propMod : m_propMods) {
		propMod->serialize(strVec, level, false);
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
	ss << "\t// COMBAdd //\n";
	ss << "\tif (d" << (level + 1) << " < d" << level << ") {\n";
	ss << "\t\td" << level << " = d" << (level + 1) << ";\n";
	ss << "\t\tcolor" << level << " = color" << (level + 1) << "; \n";
	ss << "\t}\n\n";
}

double COMBSubtract::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = -m_combineObj->signedDistance(p, colorOut);
	if (colorOut) *colorOut = colorIn;
	return distanceA > distanceB ? distanceA : distanceB;
}

void COMBSubtract::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	ss << "\t// COMBSubtract //\n";
	ss << "\tif (d" << level << " < -d" << (level + 1) << ") d" << level << " = -d" << (level + 1) << ";\n\n";
}

double COMBIntersect::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	if (distanceA <= distanceB) {
		return distanceB;
	}
	if (colorOut) *colorOut = colorIn;
	return distanceA;
}

void COMBIntersect::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	ss << "\t// COMBIntersect //\n";
	ss << "\tif (d" << level << " <= d" << (level + 1) << ") d" << level << " = d" << (level + 1) << ";\n\n";
}

double COMBExclude::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut)
{
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	double h = std::max(std::min(distanceA, distanceB), std::min(-distanceA, -distanceB));
	if (colorOut) {
		switch (m_colorMode)
		{
		case COMBExclude::Double:
			if (distanceA <= distanceB) *colorOut = colorIn;
			break;
		case COMBExclude::Single:
			*colorOut = colorIn;
			break;
		case COMBExclude::Internal:
			if (h >= 0.0) *colorOut = colorIn;
			break;
		case COMBExclude::InternalDouble:
			if ((h >= 0.0) != (distanceA > distanceB)) *colorOut = colorIn;
			break;
		default:
			break;
		}
	}
	return h;
}

void COMBExclude::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	std::string modeMap[]{ "Double", "Single", "Internal", "InternalDouble" };
	ss << "\t// COMBExclude - " << modeMap[m_colorMode] << " //\n";
	ss << "\tpmFloat0 = max(min(d" << level << ", d" << (level + 1) << "), min(-d" << level << ", -d" << (level + 1) << "));\n";
	switch (m_colorMode)
	{
	case COMBExclude::Double:
		ss << "\tif (d" << level << " > d" << (level + 1) << ") {\n";
		ss << "\t\tcolor" << level << " = color" << (level + 1) << "; \n";
		ss << "\t}\n";
		break;
	case COMBExclude::Internal:
		ss << "\tif (pmFloat0 < 0.0) {\n";
		ss << "\t\tcolor" << level << " = color" << (level + 1) << "; \n";
		ss << "\t}\n";
		break;
	case COMBExclude::InternalDouble:
		ss << "\tif ((pmFloat0 >= 0.0) != (d" << level << " <= d" << (level + 1) << ")) {\n";
		ss << "\t\tcolor" << level << " = color" << (level + 1) << "; \n";
		ss << "\t}\n";
		break;
	default:
		break;
	}
	ss << "\td" << level << " = pmFloat0;\n\n";
}

void COMBExclude::innerSerialize(std::vector<std::string>& strVec, size_t level) const
{
	std::stringstream ss{};
	ss << std::string(level, '\t');
	ss << getClassName();
	ss << " colorMode " << m_colorMode;
	strVec.push_back(ss.str());
}

void SmoothCombination::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
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

void SmoothCombination::innerSerialize(std::vector<std::string>& strVec, size_t level) const
{
	std::stringstream ss{};
	ss << std::string(level, '\t');
	ss << getClassName();
	ss << " smoothing " << m_smoothing;
	ss << " colorBlending " << m_colorBlending;
	ss << " colorShifting " << m_colorShifting;
	strVec.push_back(ss.str());
}

void SmoothCombination::blendColors(double alpha, Vec3 colorIn, Vec3* colorOut)
{
	alpha *= m_colorBlending;
	alpha -= 0.5 * (m_colorBlending - 1.0);
	alpha = std::clamp(alpha, 0.0, 1.0);
	*colorOut = colorIn.lerp(*colorOut, alpha);
}

double COMBAddSmooth::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	double h = std::clamp(0.5 + 0.5 * (distanceB - distanceA) / m_smoothing, 0.0, 1.0);
	if (colorOut) blendColors(std::clamp(1.0 - h + m_colorShifting, 0.0, 1.0), colorIn, colorOut);
	return std::lerp(distanceB, distanceA, h) - m_smoothing * h * (1.0 - h);
}

void COMBAddSmooth::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	std::string propName{ "" };
	ss << "\t// COMBAddSmooth //\n";
	ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "smoothing") ? propName : std::to_string(m_smoothing)) << ";\n";
	ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "colorShifting") ? propName : std::to_string(m_colorShifting)) << ";\n";
	ss << "\tpmFloat2 = " << (getShaderPropModCalls(propName = "colorBlending") ? propName : std::to_string(m_colorBlending)) << ";\n";
	ss << "\tblend = clamp(0.5 + 0.5 * (d" << (level + 1) << " - d" << level << ") / pmFloat0, 0.0, 1.0);\n";
	ss << "\tcolor" << level << " = blendColors(clamp(1.0 - blend + pmFloat1, 0.0, 1.0), color" << level << ", color" << (level + 1) << ", pmFloat2);\n";
	ss << "\td" << level << " = mix(d" << (level + 1) << ", d" << level << ", blend) - pmFloat0 * blend * (1.0 - blend);\n\n";
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
	std::string propName{ "" };
	ss << "\t// COMBSubtractSmooth //\n";
	ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "smoothing") ? propName : std::to_string(m_smoothing)) << ";\n";
	ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "colorShifting") ? propName : std::to_string(m_colorShifting)) << ";\n";
	ss << "\tpmFloat2 = " << (getShaderPropModCalls(propName = "colorBlending") ? propName : std::to_string(m_colorBlending)) << ";\n";
	ss << "\tblend = pmFloat0 - abs(-d" << (level + 1) << " - d" << level << ");\n";
	ss << "\tblend = max(blend, 0.0);\n";
	ss << "\tcolor" << level << " = blendColors(clamp(blend + pmFloat1, 0.0, 1.0), color" << level << ", color" << (level + 1) << ", pmFloat2);\n";
	ss << "\td" << level << " = (d" << level << " > -d" << (level + 1) << " ? d" << level << " : -d" << (level + 1) << ") + blend * blend * 0.25 / pmFloat0;\n\n";
}

double COMBIntersectSmooth::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	double h = std::clamp(0.5 - 0.5 * (distanceB - distanceA) / m_smoothing, 0.0, 1.0);
	if (colorOut) blendColors(std::clamp(h + m_colorShifting, 0.0, 1.0), colorIn, colorOut);
	return std::lerp(distanceB, distanceA, h) + m_smoothing * h * (1.0 - h);
}

void COMBIntersectSmooth::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	std::string propName{ "" };
	ss << "\t// COMBIntersectSmooth //\n";
	ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "smoothing") ? propName : std::to_string(m_smoothing)) << ";\n";
	ss << "\tpmFloat1 = " << (getShaderPropModCalls(propName = "colorShifting") ? propName : std::to_string(m_colorShifting)) << ";\n";
	ss << "\tpmFloat2 = " << (getShaderPropModCalls(propName = "colorBlending") ? propName : std::to_string(m_colorBlending)) << ";\n";
	ss << "\tblend = clamp(0.5 - 0.5 * (d" << (level + 1) << " - d" << level << ") / pmFloat0, 0.0, 1.0);\n";
	ss << "\tcolor" << level << " = blendColors(clamp(blend + pmFloat1, 0.0, 1.0), color" << level << ", color" << (level + 1) << ", pmFloat2);\n";
	ss << "\td" << level << " = mix(d" << (level + 1) << ", d" << level << ", blend) + pmFloat0 * blend * (1.0 - blend);\n\n";
}