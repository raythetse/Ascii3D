#pragma once
#include <algorithm>
#include <memory>
#include "Vec2.h"
#include "Vec3.h"

class VisualObject;

namespace SDFOp
{
	enum SDFOperationType {
		Combine,
		Surface,
		Repeat,
		Transform
	};

	class SDFOperation
	{
	public:
		void execute(double& distance, Vec3& p, double time, const Color colorIn, Color* colorOut);
		virtual SDFOperationType opType() = 0;
	};

	class CombinationOperation : public SDFOperation
	{
	protected:
		double m_smoothing;
		double m_colorBlending;
		std::unique_ptr<VisualObject> m_combineObj;
		void blendColors(double blendRate, Vec3 colorIn, Vec3* colorOut);
		virtual double combine(double a, double b, Color colorIn, Color* colorOut) = 0;
	public:
		CombinationOperation(std::unique_ptr<VisualObject> combineObj, double smoothing = 0.5, double colorBlending = 1.0)
			: m_combineObj(std::move(combineObj)), m_smoothing(smoothing), m_colorBlending(colorBlending) {}
		SDFOperationType opType() override { return SDFOperationType::Combine; }
		friend void SDFOperation::execute(double& distance, Vec3& p, double time, const Vec3 colorIn, Vec3* colorOut);
	};

	class Add : public CombinationOperation
	{
	protected:
		double combine(double, double b, Color colorIn, Color* colorOut) noexcept override;
	public:
		Add(std::unique_ptr<VisualObject> combineObj) : CombinationOperation(std::move(combineObj)) {}
	};

	class Subtract : public CombinationOperation
	{
	protected:
		double combine(double, double b, Color colorIn, Color* colorOut) noexcept override;
	public:
		Subtract(std::unique_ptr<VisualObject> combineObj) : CombinationOperation(std::move(combineObj)) {}
	};

	class Intersect : public CombinationOperation
	{
	protected:
		double combine(double, double b, Color colorIn, Vec3* colorOut) noexcept override;
	public:
		Intersect(std::unique_ptr<VisualObject> combineObj) : CombinationOperation(std::move(combineObj)) {}
	};

	class AddSmooth : public CombinationOperation
	{
	protected:
		double combine(double, double b, Color colorIn, Color* colorOut) noexcept override;
	public:
		AddSmooth(std::unique_ptr<VisualObject> combineObj, double smoothing = 0.5, double colorBlending = 1.0)
			: CombinationOperation(std::move(combineObj), smoothing, colorBlending) {}
	};

	class SubtractSmooth : public CombinationOperation
	{
	protected:
		double combine(double, double b, Color colorIn, Color* colorOut) noexcept override;
	public:
		SubtractSmooth(std::unique_ptr<VisualObject> objectPtr, double smoothing = 0.5, double colorBlending = 1.0)
			: CombinationOperation(std::move(objectPtr), smoothing, colorBlending) {}
	};

	class IntersectSmooth : public CombinationOperation
	{
	protected:
		double combine(double, double b, Color colorIn, Color* colorOut) noexcept override;
	public:
		IntersectSmooth(std::unique_ptr<VisualObject> objectPtr, double smoothing = 0.5, double colorBlending = 1.0)
			: CombinationOperation(std::move(objectPtr), smoothing, colorBlending) {}
	};

	class SurfaceOperation : public SDFOperation
	{
	protected:
		double m_magnitude{};
		virtual double resurface(double distance) = 0;
	public:
		SurfaceOperation(double magnitude = 0.01) : m_magnitude(magnitude) {}
		SDFOperationType opType() override { return SDFOperationType::Surface; }
		friend void SDFOperation::execute(double& distance, Vec3& p, double time, const Color colorIn, Color* colorOut);
	};

	class Onion : public SurfaceOperation
	{
	protected:
		double resurface(double distance) override;
	public:
		Onion(double magnitude = 0.01) : SurfaceOperation(magnitude) {}
	};

	class RepeatOperation : public SDFOperation
	{
	protected:
		double m_spacing;
		virtual void repeat(Vec3& p) = 0;
	public:
		RepeatOperation(double spacing = 1.0) : m_spacing(spacing) {};
		SDFOperationType opType() override { return SDFOperationType::Repeat; }
		friend void SDFOperation::execute(double& distance, Vec3& p, double time, const Color colorIn, Color* colorOut);
	};

	class MirrorX : public RepeatOperation
	{
	protected:
		void repeat(Vec3& p) noexcept override;
	public:
		MirrorX() {};
	};

	class MirrorY : public RepeatOperation
	{
	protected:
		void repeat(Vec3& p) noexcept override;
	public:
		MirrorY() {};
	};

	class MirrorZ : public RepeatOperation
	{
	protected:
		void repeat(Vec3& p) noexcept override;
	public:
		MirrorZ() {};
	};

	class MirrorXY : public RepeatOperation
	{
	protected:
		void repeat(Vec3& p) noexcept override;
	public:
		MirrorXY() {};
	};

	class MirrorXZ : public RepeatOperation
	{
	protected:
		void repeat(Vec3& p) noexcept override;
	public:
		MirrorXZ() {};
	};

	class MirrorYZ : public RepeatOperation
	{
	protected:
		void repeat(Vec3& p) noexcept override;
	public:
		MirrorYZ() {};
	};

	class MirrorXYZ : public RepeatOperation
	{
	protected:
		void repeat(Vec3& p) noexcept override;
	public:
		MirrorXYZ() {};
	};
}