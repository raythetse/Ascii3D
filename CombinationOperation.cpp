#include "CombinationOperation.h"
#include <algorithm>
#include <sstream>

void CombinationOperation::setCombineObject(std::unique_ptr<SDFObject> combineObj)
{
	m_combineObj = std::move(combineObj);
}

void CombinationOperation::update()
{
	m_isOn = m_offTime <= 0.0 || std::fmod(GameTimer::gameTime() + m_phase, m_onTime + m_offTime) <= m_onTime;
	if (m_isOn) {
		for (auto& an : m_animations) {
			an->update();
		}
		innerUpdate();
		m_combineObj->update();
	}
}

void CombinationOperation::resetAnimations()
{
	for (auto& an : m_animations) {
		an->resetProperty();
	}
	m_combineObj->resetAnimations();
}

void CombinationOperation::serialize(std::vector<std::string>& strVec, size_t level) const
{
	for (auto& an : m_animations) {
		an->resetProperty();
	}
	innerSerialize(strVec, level);
	for (auto& an : m_animations) {
		an->serialize(strVec, level);
	}
	m_combineObj->serialize(strVec, level);
}

void CombinationOperation::getShaderCalls(std::stringstream& ss, size_t level) const
{
	if (m_offTime > 0.0) {
		ss << "if (isEnabled(" << m_onTime << ", " << m_offTime << ", " << m_phase << ", time)) {\n";
	}
	m_combineObj->getShaderCalls(ss, level + 1);
	getShaderOpCalls(ss, level);
	if (m_offTime > 0.0) {
		ss << "}\n";
	}
}

void CombinationOperation::innerSerialize(std::vector<std::string>& strVec, size_t level) const
{
	std::stringstream ss{};
	ss << std::string(level, '\t');
	ss << getClassName();
	ss << " onTime " << m_onTime;
	ss << " offTime " << m_offTime;
	ss << " phase " << m_phase;
	strVec.push_back(ss.str());
}

void CombinationOperation::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	if (m_isOn) {
		combine(p, distanceA, colorIn, colorOut);
	}
}

void COMBAdd::combine(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	if (distanceA <= distanceB) {
		if (colorOut && !m_overwriteColor) *colorOut = colorIn;
		return;
	}
	distanceA = distanceB;
}

void COMBAdd::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	ss << "\t// COMBAdd //\n";
	ss << "\tif (d" << (level + 1) << " < d" << level << ") {\n";
	ss << "\t\td" << level << " = d" << (level + 1) << ";\n";
	ss << "\t\tcolor" << level << " = color" << (level + 1) << "; \n";
	ss << "\t}\n";
	ss << "\telse if (" << (m_overwriteColor ? "true" : "false") << ") {\n";
	ss << "\t\tcolor" << level << " = color" << (level + 1) << "; \n";
	ss << "\t}\n\n";
}

void COMBSubtract::combine(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	double distanceB = -m_combineObj->signedDistance(p, colorOut);
	if (colorOut) *colorOut = colorIn;
	distanceA = distanceA > distanceB ? distanceA : distanceB;
}

void COMBSubtract::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	ss << "\t// COMBSubtract //\n";
	ss << "\tif (d" << level << " < -d" << (level + 1) << ") d" << level << " = -d" << (level + 1) << ";\n\n";
}

void COMBIntersect::combine(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	if (distanceA <= distanceB) {
		distanceA = distanceB;
	}
	if (colorOut) *colorOut = colorIn;
}

void COMBIntersect::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	ss << "\t// COMBIntersect //\n";
	ss << "\tif (d" << level << " <= d" << (level + 1) << ") d" << level << " = d" << (level + 1) << ";\n\n";
}

void COMBExclude::combine(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	double h = std::max(std::min(distanceA, distanceB), std::min(-distanceA, -distanceB));
	if (colorOut) {
		switch (m_colorMode)
		{
		case COLORMODE_DOUBLE:
			if (distanceA <= distanceB) *colorOut = colorIn;
			break;
		case COLORMODE_SINGLE:
			*colorOut = colorIn;
			break;
		case COLORMODE_INTERNAL:
			if (h >= 0.0) *colorOut = colorIn;
			break;
		case COLORMODE_INTERNALDOUBLE:
			if ((h >= 0.0) != (distanceA > distanceB)) *colorOut = colorIn;
			break;
		default:
			break;
		}
	}
	distanceA = h;
}

void COMBExclude::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	std::string modeMap[]{ "Double", "Single", "Internal", "InternalDouble" };
	ss << "\t// COMBExclude - " << modeMap[m_colorMode] << " //\n";
	ss << "\tpmFloat0 = max(min(d" << level << ", d" << (level + 1) << "), min(-d" << level << ", -d" << (level + 1) << "));\n";
	switch (m_colorMode)
	{
	case COLORMODE_DOUBLE:
		ss << "\tif (d" << level << " > d" << (level + 1) << ") {\n";
		ss << "\t\tcolor" << level << " = color" << (level + 1) << "; \n";
		ss << "\t}\n";
		break;
	case COLORMODE_INTERNAL:
		ss << "\tif (pmFloat0 < 0.0) {\n";
		ss << "\t\tcolor" << level << " = color" << (level + 1) << "; \n";
		ss << "\t}\n";
		break;
	case COLORMODE_INTERNALDOUBLE:
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

void SmoothCombination::bindAnimation(std::unique_ptr<Animation> animation)
{
	if (animation->propName() == "smoothing") {
		animation->BindToProperty(&m_smoothing);
		m_animations.push_back(std::move(animation));
	}
	else if (animation->propName() == "colorBlending") {
		animation->BindToProperty(&m_colorBlending);
		m_animations.push_back(std::move(animation));
	}
	else if (animation->propName() == "colorShifting") {
		animation->BindToProperty(&m_colorShifting);
		m_animations.push_back(std::move(animation));
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

void SmoothCombination::blendColors(double alpha, Vec3 colorIn, Vec3* colorOut) const
{
	alpha *= m_colorBlending;
	alpha -= 0.5 * (m_colorBlending - 1.0);
	alpha = std::clamp(alpha, 0.0, 1.0);
	*colorOut = colorIn.lerp(*colorOut, alpha);
}

void COMBAddSmooth::combine(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	double h = std::clamp(0.5 + 0.5 * (distanceB - distanceA) / m_smoothing, 0.0, 1.0);
	if (colorOut) blendColors(std::clamp(1.0 - h + m_colorShifting, 0.0, 1.0), colorIn, colorOut);
	distanceA = std::lerp(distanceB, distanceA, h) - m_smoothing * h * (1.0 - h);
}

void COMBAddSmooth::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	std::string propName{ "" };
	ss << "\t// COMBAddSmooth //\n";
	ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "smoothing") ? propName : std::to_string(m_smoothing)) << ";\n";
	ss << "\tpmFloat1 = " << (getShaderAnimationCalls(propName = "colorShifting") ? propName : std::to_string(m_colorShifting)) << ";\n";
	ss << "\tpmFloat2 = " << (getShaderAnimationCalls(propName = "colorBlending") ? propName : std::to_string(m_colorBlending)) << ";\n";
	ss << "\tblend = clamp(0.5 + 0.5 * (d" << (level + 1) << " - d" << level << ") / pmFloat0, 0.0, 1.0);\n";
	ss << "\tcolor" << level << " = blendColors(clamp(1.0 - blend + pmFloat1, 0.0, 1.0), color" << level << ", color" << (level + 1) << ", pmFloat2);\n";
	ss << "\td" << level << " = mix(d" << (level + 1) << ", d" << level << ", blend) - pmFloat0 * blend * (1.0 - blend);\n\n";
}

void COMBSubtractSmooth::combine(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	double h = m_smoothing - abs(-distanceB - distanceA);
	h = h < 0.0 ? 0.0 : h;
	if (colorOut) blendColors(std::clamp(h + m_colorShifting, 0.0, 1.0), colorIn, colorOut);
	distanceA = (distanceA > -distanceB ? distanceA : -distanceB) + h * h * 0.25 / m_smoothing;
}

void COMBSubtractSmooth::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	std::string propName{ "" };
	ss << "\t// COMBSubtractSmooth //\n";
	ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "smoothing") ? propName : std::to_string(m_smoothing)) << ";\n";
	ss << "\tpmFloat1 = " << (getShaderAnimationCalls(propName = "colorShifting") ? propName : std::to_string(m_colorShifting)) << ";\n";
	ss << "\tpmFloat2 = " << (getShaderAnimationCalls(propName = "colorBlending") ? propName : std::to_string(m_colorBlending)) << ";\n";
	ss << "\tblend = pmFloat0 - abs(-d" << (level + 1) << " - d" << level << ");\n";
	ss << "\tblend = max(blend, 0.0);\n";
	ss << "\tcolor" << level << " = blendColors(clamp(blend + pmFloat1, 0.0, 1.0), color" << level << ", color" << (level + 1) << ", pmFloat2);\n";
	ss << "\td" << level << " = (d" << level << " > -d" << (level + 1) << " ? d" << level << " : -d" << (level + 1) << ") + blend * blend * 0.25 / pmFloat0;\n\n";
}

void COMBIntersectSmooth::combine(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	double h = std::clamp(0.5 - 0.5 * (distanceB - distanceA) / m_smoothing, 0.0, 1.0);
	if (colorOut) blendColors(std::clamp(h + m_colorShifting, 0.0, 1.0), colorIn, colorOut);
	distanceA = std::lerp(distanceB, distanceA, h) + m_smoothing * h * (1.0 - h);
}

void COMBIntersectSmooth::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	std::string propName{ "" };
	ss << "\t// COMBIntersectSmooth //\n";
	ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "smoothing") ? propName : std::to_string(m_smoothing)) << ";\n";
	ss << "\tpmFloat1 = " << (getShaderAnimationCalls(propName = "colorShifting") ? propName : std::to_string(m_colorShifting)) << ";\n";
	ss << "\tpmFloat2 = " << (getShaderAnimationCalls(propName = "colorBlending") ? propName : std::to_string(m_colorBlending)) << ";\n";
	ss << "\tblend = clamp(0.5 - 0.5 * (d" << (level + 1) << " - d" << level << ") / pmFloat0, 0.0, 1.0);\n";
	ss << "\tcolor" << level << " = blendColors(clamp(blend + pmFloat1, 0.0, 1.0), color" << level << ", color" << (level + 1) << ", pmFloat2);\n";
	ss << "\td" << level << " = mix(d" << (level + 1) << ", d" << level << ", blend) + pmFloat0 * blend * (1.0 - blend);\n\n";
}