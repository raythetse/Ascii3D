#pragma once
#include <algorithm>
#include <memory>
#include "Mat3.h"
#include "MathFunctions.h"
#include "PropertyModifier.h"
#include "Vec2.h"
#include "Vec3.h"

enum SDFOperationType {
	Combine,
	Resurface,
	Transform,
};

class SDFOperation
{
protected:
	std::vector<std::unique_ptr<PropertyModifierBase>> m_propMods{};
	virtual void innerUpdate() = 0;
	virtual double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) { return {}; }
	virtual double resurface(double distance) { return distance; };
	virtual void transform(Vec3& p) const {};
public:
	virtual void update();
	void execute(Vec3& p, double& distance, const Color colorIn, Color* colorOut);
	virtual SDFOperationType opType() = 0;
	virtual void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) = 0;
};
