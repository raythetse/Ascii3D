#pragma once
#include "SDFOperation.h"

void SDFOperation::update()
{
	for (auto& an : m_animations) {
		an->update();
	}
	innerUpdate();
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

void SDFOperation::getShaderCalls(std::stringstream& ss, size_t level) const
{
	getShaderOpCalls(ss, level);
}

bool SDFOperation::getShaderAnimationCalls(std::string& propName) const
{
	for (auto& an : m_animations) {
		if (an->getShaderCall(propName)) return true;
	}
	return false;
}
