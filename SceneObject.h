#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include "Behaviour.h"
#include "Vec2.h"
#include "Vec3.h"

namespace SDFOp
{
	class SDFOperation;
}

class SceneObject
{
protected:
	Vec3 m_startPosition;
	Vec3 m_currentPosition;
	Mat3 m_rotationMat{};
	bool m_hasStartRotation{};
	Vec3 m_color;
	bool m_lit;
	double m_rounding;
	std::unique_ptr<Behaviour> m_behaviour;
	std::vector<std::unique_ptr<SDFOp::SDFOperation>> m_operations;
	SceneObject(Vec3 position, Vec3 rotation, Vec3 color, double rounding, bool lit);
	virtual double signedDistanceInner(Vec3 p, double time) const = 0;
public:
	void update(double time);
	~SceneObject();
	Vec3 position() const;
	double signedDistance(Vec3 p, double time, Vec3* colorOut = nullptr) const;
	bool lit() const;
	void addOperation(std::unique_ptr<SDFOp::SDFOperation> operation);
private:
};

class Box : public SceneObject
{
private:
	Vec3 m_dimensions;
public:
	Box(Vec3 position, Vec3 rotation, Vec3 dimensions, Vec3 color = Color::White, double rounding = 0.0, bool lit = true);
	double signedDistanceInner(Vec3 p, double time) const override;
};

class Sphere : public SceneObject
{
public:
	Sphere(Vec3 position, double radius, Vec3 color = Color::White, bool lit = true);
	double signedDistanceInner(Vec3 p, double time) const override;
};

class Torus : public SceneObject
{
private:
	Vec2 m_radii;
public:
	Torus(Vec3 position, Vec3 rotation, Vec2 radii, Vec3 color = Color::White, bool lit = true);
	double signedDistanceInner(Vec3 p, double time) const override;
};

