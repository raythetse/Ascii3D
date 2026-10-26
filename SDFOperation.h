#pragma once
#include <algorithm>
#include <memory>
#include <sstream>
#include "Enumerations.h"
#include "Mat3.h"
#include "MathFunctions.h"
#include "Animation.h"
#include "Vec2.h"
#include "Vec3.h"

// Operations for building an SDF scene tree. Operations can perform combinations, transformations, repetitions, and other functions.
class SDFOperation
{
protected:
	std::vector<std::unique_ptr<Animation>> m_animations{};
	virtual void innerUpdate() = 0;
	virtual void innerSerialize(std::vector<std::string>& strVec, size_t level) const = 0;
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const = 0;
	bool getShaderAnimationCalls(std::string& propName) const;
	SDFOperation() = default;
public:
	virtual void update();
	virtual void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const = 0;
	virtual void bindAnimation(std::unique_ptr<Animation> animation) = 0;
	virtual void resetAnimations();
	virtual void serialize(std::vector<std::string>& strVec, size_t level) const;
	virtual void getShaderCalls(std::stringstream& ss, size_t level) const;
	virtual ~SDFOperation() = default;
};
