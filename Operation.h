#pragma once
#include <algorithm>
#include <memory>
#include "Vec2.h"
#include "Vec3.h"

class SceneObject;

namespace SDFOp
{
	enum SDFOperationType {
		Combine,
		Surface,
		Spacial,
		Transform
	};

	class SDFOperation
	{
	public:

		void execute(double& distance, Vec3 p, double time, const Vec3 colorIn, Vec3* colorOut);
		virtual SDFOperationType opType() = 0;
	};

	class CombinationOperation : public SDFOperation
	{
	protected:
		double m_smoothing;
		double m_colorBlending;
		double m_colorBlendBias;
		std::unique_ptr<SceneObject> m_combineObj;
	public:
		CombinationOperation(std::unique_ptr<SceneObject> combineObj, double smoothing = 0.5, double colorBlending = 1.0, double colorBlendBias = 1.0)
			: m_combineObj(std::move(combineObj)), m_smoothing(smoothing), m_colorBlending(colorBlending), m_colorBlendBias(colorBlendBias) {}
		virtual void combine(Vec2& a, double b) = 0;
		SDFOperationType opType() override { return SDFOperationType::Combine; }
		friend void SDFOperation::execute(double& distance, Vec3 p, double time, const Vec3 colorIn, Vec3* colorOut);
	};

	class Add : public CombinationOperation
	{
	protected:
		void combine(Vec2& a, double b) noexcept override;
	public:
		Add(std::unique_ptr<SceneObject> combineObj) : CombinationOperation(std::move(combineObj)) {}
	};

	class Subtract : public CombinationOperation
	{
	protected:
		void combine(Vec2& a, double b) noexcept override;
	public:
		Subtract(std::unique_ptr<SceneObject> combineObj) : CombinationOperation(std::move(combineObj)) {}
	};

	class Intersect : public CombinationOperation
	{
	protected:
		void combine(Vec2& a, double b) noexcept override;
	public:
		Intersect(std::unique_ptr<SceneObject> combineObj) : CombinationOperation(std::move(combineObj)) {}
	};

	class JoinSmooth : public CombinationOperation
	{
	protected:
		void combine(Vec2& a, double b) noexcept override;
	public:
		JoinSmooth(std::unique_ptr<SceneObject> combineObj, double smoothing = 0.5, double colorBlending = 1.0, double coloBlendBias = 1.0)
			: CombinationOperation(std::move(combineObj), smoothing, colorBlending, coloBlendBias) {}
	};

	class SubtractSmooth : public CombinationOperation
	{
	protected:
		void combine(Vec2& a, double b) noexcept override;
	public:
		SubtractSmooth(std::unique_ptr<SceneObject> objectPtr, double smoothing = 0.5, double colorBlending = 1.0, double colorBlendBias = 1.0)
			: CombinationOperation(std::move(objectPtr), smoothing, colorBlending, colorBlendBias) {}
	};

	class IntersectSmooth : public CombinationOperation
	{
	protected:
		void combine(Vec2& a, double b) noexcept override;
	public:
		IntersectSmooth(std::unique_ptr<SceneObject> objectPtr, double smoothing = 0.5, double colorBlending = 1.0, double colorBlendBias = 1.0)
			: CombinationOperation(std::move(objectPtr), smoothing, colorBlending, colorBlendBias) {}
	};

	class SurfaceOperation : public SDFOperation
	{
	protected:
		double m_magnitude{};
	public:
		SurfaceOperation(double magnitude = 0.01) : m_magnitude(magnitude) {}
		virtual double resurface(double distance) = 0;
		SDFOperationType opType() override { return SDFOperationType::Surface; }
		friend void SDFOperation::execute(double& distance, Vec3 p, double time, const Vec3 colorIn, Vec3* colorOut);
	};

	class Onion : public SurfaceOperation
	{
	protected:
		double resurface(double distance) override;
	public:
		Onion(double magnitude = 0.01) : SurfaceOperation(magnitude) {}
	};
}