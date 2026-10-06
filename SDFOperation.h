#pragma once
#include <algorithm>
#include <memory>
#include <sstream>
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
	SDFOperationType m_opType;
	std::vector<std::unique_ptr<PropertyModifierBase>> m_propMods{};
	SDFOperation(SDFOperationType opType) : m_opType(opType) {}
	virtual void innerUpdate() = 0;
	virtual double combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) { return distanceA; }
	virtual double resurface(double distance) { return distance; };
	virtual void transform(Vec3& p) const {};
	virtual void innerSerialize(std::vector<std::string>& strVec, size_t level) const = 0;
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const = 0;
	bool getShaderPropModCalls(std::string& propName) const;
public:
	virtual void update();
	void execute(Vec3& p, double& distance, const Color colorIn, Color* colorOut);
	virtual void bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod) = 0;
	virtual void serialize(std::vector<std::string>& strVec, size_t level) const;
	virtual void getShaderCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const;
};
