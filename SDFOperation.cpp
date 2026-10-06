#pragma once
#include "SDFOperation.h"

void SDFOperation::update()
{
	for (auto& propMod : m_propMods) {
		propMod->update();
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

void SDFOperation::serialize(std::vector<std::string>& strVec, size_t level) const
{
	for (auto& propMod : m_propMods) {
		propMod->resetProperty();
	}
	innerSerialize(strVec, level);
	for (auto& propMod : m_propMods) {
		propMod->serialize(strVec, level, false);
	}
}

void SDFOperation::getShaderCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	getShaderOpCalls(ss, level, maxLevel);
}

bool SDFOperation::getShaderPropModCalls(std::string& propName) const
{
	for (auto& propMod : m_propMods) {
		if (propMod->getShaderCall(propName)) return true;
	}
	return false;
}
