#pragma once
#include "SDFOperation.h"

// Alters the previously calculated point distance
class ResurfaceOperation : public SDFOperation
{
public:
	inline static const double DFLT_MAGNITUDE{ 0.01 };
protected:
	double m_magnitude{};
	void innerUpdate() override {};
	virtual std::string getClassName() const = 0;
	void innerSerialize(std::vector<std::string>& strVec, size_t level) const override;
	ResurfaceOperation(double magnitude) : m_magnitude(magnitude) {}
public:
	BindResult bindAnimation(std::unique_ptr<Animation> animation) override;
	virtual ~ResurfaceOperation() = default;
};

// Rounds off edges and corners of previously added objects
class OPRoundOff : public ResurfaceOperation
{
protected:
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const;
	std::string getClassName() const { return "OPRoundOff"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPRoundOff(double magnitude = DFLT_MAGNITUDE) : ResurfaceOperation(magnitude) {}
};

// Creates a layed effect on the inside of objects.  Use with subtract combinations to reveal the inside.
class OPOnion : public ResurfaceOperation
{
protected:
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const;
	std::string getClassName() const { return "OPOnion"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPOnion(double magnitude = DFLT_MAGNITUDE) : ResurfaceOperation(magnitude) {}
};

// Swaps occupied space with empty space.  For example, turns a box into a rectangular room with infinitely thick walls surrounding it.
class OPInsideOut : public ResurfaceOperation
{
protected:
	void execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const;
	std::string getClassName() const { return "OPOnion"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level) const override;
public:
	OPInsideOut() : ResurfaceOperation(0.0) {}
};
