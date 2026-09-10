#pragma once
#include <algorithm>
#include <memory>
#include "Mat3.h"
#include "MathFunctions.h"
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
	virtual double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) { return {}; }
	virtual double resurface(double distance) { return distance; };
	virtual void transform(Vec3& p) const {};
public:
	void execute(Vec3& p, double& distance, const Color colorIn, Color* colorOut);
	virtual SDFOperationType opType() = 0;
	virtual void* getPropertyPtr(std::string_view propName) { return nullptr; };
};