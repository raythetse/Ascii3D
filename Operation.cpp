#pragma once
#include "Operation.h"
#include "SceneObject.h"

namespace SDFOp
{
	void SDFOperation::execute(double& distance, Vec3& p, double time, const Vec3 colorIn, Vec3* colorOut)
	{
		switch (opType())
		{
		case SDFOperationType::Combine:{
			Vec2 v{ distance, 0.0 };
			CombinationOperation* comOp = dynamic_cast<CombinationOperation*>(this);
			distance = comOp->combine(distance, comOp->m_combineObj->signedDistance(p, time, colorOut));
			return;
		}
		case SDFOperationType::Surface: {
			SurfaceOperation* surfOp = dynamic_cast<SurfaceOperation*>(this);
			distance = surfOp->resurface(distance);
			return;
		}
		case SDFOperationType::Repeat: {
			RepeatOperation* repOp = dynamic_cast<RepeatOperation*>(this);
			repOp->repeat(p);
			return;
		}
		case SDFOperationType::Transform: {
			return;
		}
		default:
			break;
		}
	}

	double Add::combine(double a, double b) noexcept {
		return a < b ? a : b;
	}

	double Subtract::combine(double a, double b) noexcept {
		return a > -b ? a : -b;
	}

	double Intersect::combine(double a, double b) noexcept {
		return a > b ? a : b;
	}

	double JoinSmooth::combine(double a, double b) noexcept {
		double h = std::clamp(0.5 + 0.5 * (b - a) / m_smoothing, 0.0, 1.0);
		return std::lerp(b, a, h) - m_smoothing * h * (1.0 - h);
		//a.y = 1.0 - h;
	}

	double SubtractSmooth::combine(double a, double b) noexcept {
		double h = m_smoothing - abs(-b - a);
		h = h < 0.0 ? 0.0 : h;
		return (a > -b ? a : -b) + h * h * 0.25 / m_smoothing;
	}

	double IntersectSmooth::combine(double a, double b) noexcept {
		double h = std::clamp(0.5 - 0.5 * (b - a) / m_smoothing, 0.0, 1.0);
		return std::lerp(b, a, h) + m_smoothing * h * (1.0 - h);
		//a.y = 1.0 - h;
	}

	double Onion::resurface(double distance) {
		return abs(distance) - m_magnitude;
	}

	void MirrorX::repeat(Vec3& p) noexcept
	{
		p.x = abs(p.x);
	}

	void MirrorY::repeat(Vec3& p) noexcept
	{
		p.y = abs(p.x);
	}

	void MirrorZ::repeat(Vec3& p) noexcept
	{
		p.z = abs(p.z);
	}

	void MirrorXY::repeat(Vec3& p) noexcept
	{
		p.x = abs(p.x);
		p.y = abs(p.y);
	}

	void MirrorXZ::repeat(Vec3& p) noexcept
	{
		p.x = abs(p.x);
		p.z = abs(p.z);
	}

	void MirrorYZ::repeat(Vec3& p) noexcept
	{
		p.y = abs(p.y);
		p.z = abs(p.z);
	}
}