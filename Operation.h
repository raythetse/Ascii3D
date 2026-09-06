#pragma once
#include <algorithm>
#include "Vec2.h"
#include "Vec3.h"

class SceneObject;

class Operation
{
public:
	enum OpType {
		Combination,
		Surface,
		Spacial,
		Transform
	};
	Operation(OpType opType) : m_opType(opType) {};
protected:
	OpType m_opType{};
	virtual void makeMePolymorphic() {}; // Probably means I should find a different solution?
public:
	void execute(double& distance, Vec3 p, double time, const Vec3 colorIn, Vec3* colorOut);
};

namespace OpCombine
{
	class CombineOperation : public Operation
	{
	protected:
		double m_smoothing;
		double m_colorBlending;
		double m_colorBlendBias;
		std::unique_ptr<SceneObject> m_combineObj;
	public:
		CombineOperation(std::unique_ptr<SceneObject> combineObj, double smoothing = 0.5, double colorBlending = 1.0, double colorBlendBias = 1.0) 
			: Operation(OpType::Combination), m_combineObj(std::move(combineObj)), m_smoothing(smoothing), m_colorBlending(colorBlending), m_colorBlendBias(colorBlendBias) {}
		virtual void combine(Vec2& a, double b) = 0;
		friend void Operation::execute(double& distance, Vec3 p, double time, const Vec3 colorIn, Vec3* colorOut);
	};

	class Join : public CombineOperation
	{
	protected:
		void combine(Vec2& a, double b) noexcept override;
	public:
		Join(std::unique_ptr<SceneObject> combineObj) : CombineOperation(std::move(combineObj)) {}
	};

	class Subtract : public CombineOperation
	{
	protected:
		void combine(Vec2& a, double b) noexcept override;
	public:
		Subtract(std::unique_ptr<SceneObject> combineObj) : CombineOperation(std::move(combineObj)) {}
	};

	class Intersect : public CombineOperation
	{
	protected:
		void combine(Vec2& a, double b) noexcept override;
	public:
		Intersect(std::unique_ptr<SceneObject> combineObj) : CombineOperation(std::move(combineObj)) {}
	};

	class JoinSmooth : public CombineOperation
	{
	protected:
		void combine(Vec2& a, double b) noexcept override;
	public:
		JoinSmooth(std::unique_ptr<SceneObject> combineObj, double smoothing = 0.5, double colorBlending = 1.0, double coloBlendBias = 1.0)
			: CombineOperation(std::move(combineObj), smoothing, colorBlending, coloBlendBias) {}
	};

	class SubtractSmooth : public CombineOperation
	{
	protected:
		void combine(Vec2& a, double b) noexcept override;
	public:
		SubtractSmooth(std::unique_ptr<SceneObject> objectPtr, double smoothing = 0.5, double colorBlending = 1.0, double colorBlendBias = 1.0)
			: CombineOperation(std::move(objectPtr), smoothing, colorBlending, colorBlendBias) {}
	};

	class IntersectSmooth : public CombineOperation
	{
	protected:
		void combine(Vec2& a, double b) noexcept override;
	public:
		IntersectSmooth(std::unique_ptr<SceneObject> objectPtr, double smoothing = 0.5, double colorBlending = 1.0, double colorBlendBias = 1.0)
			: CombineOperation(std::move(objectPtr), smoothing, colorBlending, colorBlendBias) {}
	};
}

namespace OpSurface
{
	class SurfaceOperation : public Operation
	{
	protected:
		double m_magnitude{};
	public:
		SurfaceOperation(double magnitude = 0.01) : Operation(OpType::Surface), m_magnitude(magnitude) {}
		virtual double resurface(double distance) = 0;
		friend void Operation::execute(double& distance, Vec3 p, double time, const Vec3 colorIn, Vec3* colorOut);
	};

	class Onion : public SurfaceOperation
	{
	protected:
		double resurface(double distance) override;
	public:
		Onion(double magnitude = 0.01) : SurfaceOperation(magnitude) {}
	};
}

namespace OpTransform
{

}

