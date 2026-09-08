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

// Scene objects use 'Signed-Distance Fields' (SDFs) to build out a scene
// For more information on SDFs: https://iquilezles.org/articles/distfunctions/
// Most (if not all) SDF functions are adapted from this article.
class SceneObject
{
public:
protected:
	size_t m_objId;
	Vec3 m_startPosition{};
	Mat3 m_rotationMat{};
	Color m_color;
	bool m_lit;
	std::unique_ptr<Behaviour> m_behaviour;
	std::vector<std::unique_ptr<SDFOp::SDFOperation>> m_operations;
public:
	SceneObject(Vec3 position = Vec3::Zero, Vec3 rotation = Vec3::Zero, Color color = Color::White, bool lit = true);
	~SceneObject();
	Vec3 position() const;
	virtual double signedDistance(Vec3 p, Color* colorOut = nullptr) const;
	bool lit() const;
	void pushOperation(std::unique_ptr<SDFOp::SDFOperation> operation);
	template <typename T, typename... Args>
	SceneObject* pushOperation(Args&&... args);
};

template <typename T, typename... Args>
SceneObject* SceneObject::pushOperation(Args&&... args) {
	m_operations.push_back(std::make_unique<T>(std::forward<Args>(args)...));
	return this;
}

class VisualObject : public SceneObject
{
protected:
	VisualObject(Vec3 position, Vec3 rotation, Color color, bool lit)
		: SceneObject(position, rotation, color, lit) {}
	virtual double signedDistanceInner(Vec3 p) const = 0;
public:
	double signedDistance(Vec3 p, Color* colorOut = nullptr) const override;
};

namespace SDF
{
	class PlaneXZ : public VisualObject
	{
	public:
		PlaneXZ(double yPos = 0.0, Color color = Color::White, bool lit = true)
			: VisualObject(Vec3::Up* yPos, Vec3::Zero, color, lit) {}
		double signedDistanceInner(Vec3 p) const override;
	};

	class PlaneYZ : public VisualObject
	{
	public:
		PlaneYZ(double xPos = 0.0, Color color = Color::White, bool lit = true)
			: VisualObject(Vec3::Right* xPos, Vec3::Zero, color, lit) {}
		double signedDistanceInner(Vec3 p) const override;
	};

	class PlaneXY : public VisualObject
	{
	public:
		PlaneXY(double zPos = 0.0, Color color = Color::White, bool lit = true)
			: VisualObject(Vec3::Forward* zPos, Vec3::Zero, color, lit) {}
		double signedDistanceInner(Vec3 p) const override;
	};

	class Box : public VisualObject
	{
	private:
		Vec3 m_dimensions;
	public:
		Box(Vec3 position = Vec3::Zero, Vec3 rotation = Vec3::Zero, Vec3 dimensions = Vec3::One, Color color = Color::White, bool lit = true)
			: VisualObject(position, rotation, color, lit), m_dimensions(dimensions) {}
		double signedDistanceInner(Vec3 p) const override;
	};

	class BoxFrame : public VisualObject
	{
	private:
		Vec3 m_dimensions;
		double m_edgeThickness;
	public:
		BoxFrame(Vec3 position = Vec3::Zero, Vec3 rotation = Vec3::Zero, Vec3 dimensions = Vec3::One, double edgeThickness = 0.1, Color color = Color::White, bool lit = true)
			: VisualObject(position, rotation, color, lit), m_dimensions(dimensions), m_edgeThickness(edgeThickness) {}
		double signedDistanceInner(Vec3 p) const override;
	};

	class Sphere : public VisualObject
	{
	private:
		double m_radius;
	public:
		Sphere(Vec3 position, double radius, Color color = Color::White, bool lit = true);
		double signedDistanceInner(Vec3 p) const override;
	};

	class Torus : public VisualObject
	{
	private:
		Vec2 m_radii;
	public:
		Torus(Vec3 position = Vec3::Zero, Vec3 rotation = Vec3::Zero, Vec2 radii = Vec2{ 1.0, 0.25 }, Color color = Color::White, bool lit = true)
			: VisualObject(position, rotation, color, lit), m_radii(radii) {}
		double signedDistanceInner(Vec3 p) const override;
	};
}

