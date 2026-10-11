#pragma once
#include "SDFOperation.h"

void SDFOperation::update()
{
	for (auto& an : m_animations) {
		an->update();
	}
	innerUpdate();
}

void SDFOperation::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut)
{
	switch (m_opType)
	{
	case SDFOperationType::Combine: {
		distanceA = combine(p, distanceA, colorIn, colorOut);
		return;
	}
	case SDFOperationType::Resurface: {
		distanceA = resurface(distanceA);
		if (colorOut) *colorOut = colorIn;
		return;
	}
	case SDFOperationType::Transform: {
		transform(p);
		if (colorOut) *colorOut = colorIn;
		return;
	}
	default:
		break;
	}
}

void SDFOperation::resetAnimations()
{
	for (auto& animation : m_animations) {
		animation->resetProperty();
	}
}

void SDFOperation::serialize(std::vector<std::string>& strVec, size_t level) const
{
	innerSerialize(strVec, level);
	for (auto& an : m_animations) {
		an->serialize(strVec, level);
	}
}

void SDFOperation::getShaderCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	getShaderOpCalls(ss, level, maxLevel);
}

bool SDFOperation::getShaderAnimationCalls(std::string& propName) const
{
	for (auto& an : m_animations) {
		if (an->getShaderCall(propName)) return true;
	}
	return false;
}

void OPInsertGroup::innerSerialize(std::vector<std::string>&strVec, size_t level) const
{
	strVec.push_back(std::string(level - 1, '\t') + "OPInsertGroup " + m_groupName);
}
