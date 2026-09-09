#pragma once
#include <algorithm>
#include <memory>
#include "Mat3.h"
#include "MathFunctions.h"
#include "Vec2.h"
#include "Vec3.h"

class VisualObject;

namespace SDFOp
{
	enum SDFOperationType {
		Combine,
		Surface,
		Mirror,
		Domain,
		Transform,
		Distort,
	};

	class SDFOperation
	{
	public:
		void execute(double& distance, Vec3& p, const Color colorIn, Color* colorOut);
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
		friend void SDFOperation::execute(double& distance, Vec3& p, const Vec3 colorIn, Vec3* colorOut);
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
		friend void SDFOperation::execute(double& distance, Vec3& p, const Color colorIn, Color* colorOut);
	};

	class RoundOff : public SurfaceOperation
	{
	protected:
		double resurface(double distance) override;
	public:
		RoundOff(double magnitude = 0.1) : SurfaceOperation(magnitude) {}
	};

	class Onion : public SurfaceOperation
	{
	protected:
		double resurface(double distance) override;
	public:
		Onion(double magnitude = 0.01) : SurfaceOperation(magnitude) {}
	};

	class MirrorOperation : public SDFOperation
	{
	protected:
		virtual void reflect(Vec3& p) = 0;
	public:
		MirrorOperation() {};
		SDFOperationType opType() override { return SDFOperationType::Mirror; }
		friend void SDFOperation::execute(double& distance, Vec3& p, const Color colorIn, Color* colorOut);
	};

	class MirrorX : public MirrorOperation
	{
	protected:
		void reflect(Vec3& p) noexcept override;
	public:
		MirrorX() {};
	};

	class MirrorY : public MirrorOperation
	{
	protected:
		void reflect(Vec3& p) noexcept override;
	public:
		MirrorY() {};
	};

	class MirrorZ : public MirrorOperation
	{
	protected:
		void reflect(Vec3& p) noexcept override;
	public:
		MirrorZ() {};
	};

	class MirrorXY : public MirrorOperation
	{
	protected:
		void reflect(Vec3& p) noexcept override;
	public:
		MirrorXY() {};
	};

	class MirrorXZ : public MirrorOperation
	{
	protected:
		void reflect(Vec3& p) noexcept override;
	public:
		MirrorXZ() {};
	};

	class MirrorYZ : public MirrorOperation
	{
	protected:
		void reflect(Vec3& p) noexcept override;
	public:
		MirrorYZ() {};
	};

	class MirrorXYZ : public MirrorOperation
	{
	protected:
		void reflect(Vec3& p) noexcept override;
	public:
		MirrorXYZ() {};
	};

	class DomainOperation : public SDFOperation
	{
	protected:
		Vec3 m_spacing;
		virtual void repeat(Vec3& p) = 0;
	public:
		DomainOperation(Vec3 spacing = Vec3::One) : m_spacing(spacing) {};
		SDFOperationType opType() override { return SDFOperationType::Domain; }
		friend void SDFOperation::execute(double& distance, Vec3& p, const Color colorIn, Color* colorOut);
	};

	class RepeatX : public DomainOperation
	{
	protected:
		void repeat(Vec3& p) override;
	public:
		RepeatX(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
	};

	class RepeatY : public DomainOperation
	{
	protected:
		void repeat(Vec3& p) override;
	public:
		RepeatY(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
	};

	class RepeatZ : public DomainOperation
	{
	protected:
		void repeat(Vec3& p) override;
	public:
		RepeatZ(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
	};

	class RepeatXY : public DomainOperation
	{
	protected:
		void repeat(Vec3& p) override;
	public:
		RepeatXY(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
	};

	class RepeatXZ : public DomainOperation
	{
	protected:
		void repeat(Vec3& p) override;
	public:
		RepeatXZ(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
	};

	class RepeatYZ : public DomainOperation
	{
	protected:
		void repeat(Vec3& p) override;
	public:
		RepeatYZ(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
	};

	class RepeatXYZ : public DomainOperation
	{
	protected:
		void repeat(Vec3& p) override;
	public:
		RepeatXYZ(Vec3 spacing = Vec3::One) : DomainOperation(spacing) {}
	};

	class RepeatLimited : public DomainOperation
	{
	private:
		Vec3 m_limits;
	protected:
		void repeat(Vec3& p) override;
	public:
		RepeatLimited(Vec3 limits = Vec3::One, Vec3 spacing = Vec3::One) : DomainOperation(spacing), m_limits(limits) {}
	};

	class TransformOperation : public SDFOperation
	{
	protected:
		virtual void transform(Vec3& p) = 0;
	public:
		TransformOperation() {};
		SDFOperationType opType() override { return SDFOperationType::Transform; }
		friend void SDFOperation::execute(double& distance, Vec3& p, const Color colorIn, Color* colorOut);
	};

	class Translate : public TransformOperation
	{
	private:
		Vec3 m_translation;
	protected:
		void transform(Vec3& p) override;
	public:
		Translate(Vec3 translation = Vec3::Up) : m_translation(translation) {}
	};

	class TranslateSine : public TransformOperation
	{
	private:
		Vec3 m_axis;
		double m_amplitude;
		double m_period;
		double m_phase;
	protected:
		void transform(Vec3& p) override;
	public:
		TranslateSine(Vec3 axis = Vec3::Up, double amplitude = 1.0, double period = 4.0, double phase = 0.0) : m_axis(axis), m_amplitude(amplitude), m_period(period), m_phase(phase) {}
	};

	class TranslateLinear : public TransformOperation
	{
	private:
		Vec3 m_start;
		Vec3 m_end;
		Vec2 m_transitTimes;
		Vec2 m_endDelays;
		double m_phase;
		double m_cycleTime;
	protected:
		void transform(Vec3& p) override;
	public:
		TranslateLinear(Vec3 vector = Vec3::Up, Vec2 transitTimes = 1.0, Vec2 endDelays = 0.0, double phase = 0.0)
			: m_start(vector * -0.5), m_end(vector * 0.5), m_transitTimes(transitTimes)
			, m_endDelays(endDelays), m_phase(phase + transitTimes.x * 0.5)
			, m_cycleTime(m_transitTimes.x + m_transitTimes.y + m_endDelays.x + m_endDelays.y) {}
	};

	class CubeRollHeight : public TransformOperation
	{
	private:
		double m_cubeHeight;
		double m_period;
		double m_phase;
	protected:
		void transform(Vec3& p) override;
	public:
		CubeRollHeight(double cubeHeight = 1.0, double period = 1.0, double phase = 0.0)
			: m_cubeHeight(cubeHeight), m_period(period), m_phase(phase) {}
	};

	class Rotate : public TransformOperation
	{
	private:
		Mat3 m_rotMat;
	protected:
		void transform(Vec3& p) override;
	public:
		Rotate(Vec3 axis = Vec3::Up, double rotations = 0.5) : m_rotMat(Mat3::fromAxisAngle(axis, rotations * MF::TAU)) {}
	};

	class RotateContinuous : public TransformOperation
	{
	private:
		Vec3 m_axis;
		double m_rotationsPerSecond;
		double m_phase;
	protected:
		void transform(Vec3& p) override;
	public:
		RotateContinuous(Vec3 axis = Vec3::Up, double rotationsPerSecond = 0.25, double phase = 0.0) : m_axis(axis), m_rotationsPerSecond(rotationsPerSecond), m_phase(phase) {}
	};

	class RotateScan : public TransformOperation
	{
	private:
		Vec3 m_axis;
		double m_endAngle;
		Vec2 m_transitTimes;
		Vec2 m_endDelays;
		double m_phase;
		double m_cycleTime;
	protected:
		void transform(Vec3& p) override;
	public:
		RotateScan(Vec3 axis = Vec3::Up, double rotations = 1.0, Vec2 transitTimes = 1.0, Vec2 endDelays = 0.0, double phase = 0.0)
			: m_axis(axis), m_endAngle(rotations * MF::TAU)
			, m_transitTimes(transitTimes), m_endDelays(endDelays), m_phase(phase)
			, m_cycleTime(m_transitTimes.x + m_transitTimes.y + m_endDelays.x + m_endDelays.y) {}
	};

	class RotateDelay : public TransformOperation
	{
	private:
		Vec3 m_axis;
		double m_angle;
		double m_rotateTime;
		double m_cycleTime;
		double m_phase;
	protected:
		void transform(Vec3& p) override;
	public:
		RotateDelay(Vec3 axis = Vec3::Up, double rotations = 1.0, double rotateTime = 1.0, double delayTime = 1.0, double phase = 0.0)
		: m_axis(axis), m_angle(rotations * MF::TAU), m_rotateTime(rotateTime), m_cycleTime(rotateTime + delayTime), m_phase(phase) {}
	};

	class CubeRoller : public TransformOperation
	{
	private:
		std::vector<Vec3> m_axes;
		double m_quarterTurnTime;
		double m_phase;
	protected:
		void transform(Vec3& p) override;
	public:
		CubeRoller(std::vector<Vec3> axes, double quarterTurnTime = 1.0, double phase = 0.0) : m_axes(axes), m_quarterTurnTime(quarterTurnTime), m_phase(phase) {}
	};

	class DistortOperation : public SDFOperation
	{
	protected:
		double m_magnitude;
		virtual void distort(Vec3& p) = 0;
	public:
		DistortOperation(double magnitude) : m_magnitude(magnitude) {};
		SDFOperationType opType() override { return SDFOperationType::Distort; }
		friend void SDFOperation::execute(double& distance, Vec3& p, const Color colorIn, Color* colorOut);
	};

	class Twist : public DistortOperation
	{
	protected:
		void distort(Vec3& p);
	public:
		Twist(double turns = 1.0) : DistortOperation(turns) {}
	};

	class Bend : public DistortOperation
	{
	protected:
		void distort(Vec3& p);
	public:
		Bend(double turns = 1.0) : DistortOperation(turns) {}
	};
}