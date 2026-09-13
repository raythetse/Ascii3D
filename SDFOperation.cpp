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
