#pragma once
#include "Operation.h"
#include "SceneObject.h"

namespace SDFOp
{
	

	void SDFOperation::execute(double& distanceA, Vec3& p, double time, const Color colorIn, Color* colorOut)
	{
		switch (opType())
		{
		case SDFOperationType::Combine:{
			CombinationOperation* comOp = dynamic_cast<CombinationOperation*>(this);
			double distanceB = comOp->m_combineObj->signedDistance(p, time, colorOut);
			distanceA = comOp->combine(distanceA, distanceB, colorIn, colorOut);
			return;
		}
		case SDFOperationType::Surface: {
			SurfaceOperation* surfOp = dynamic_cast<SurfaceOperation*>(this);
			distanceA = surfOp->resurface(distanceA);
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
	}void CombinationOperation::blendColors(double alpha, Vec3 colorIn, Vec3* colorOut)
	{
		alpha *= m_colorBlending;
		alpha -= 0.5 * (m_colorBlending - 1.0);
		alpha = alpha < 0.0 ? 0.0 : alpha;
		alpha = alpha > 1.0 ? 1.0 : alpha;
		*colorOut = colorIn.lerp(*colorOut, alpha);
	}

	double Add::combine(double a, double b, Color colorIn, Color* colorOut) noexcept {
		if (colorOut) *colorOut = a <= b ? colorIn : *colorOut;
		return a < b ? a : b;
	}

	double Subtract::combine(double a, double b, Color colorIn, Color* colorOut) noexcept {
		if (colorOut) *colorOut = colorIn;
		return a > -b ? a : -b;
	}

	double Intersect::combine(double a, double b, Color colorIn, Color* colorOut) noexcept {
		if (colorOut) *colorOut = a <= b ? colorIn : *colorOut;
		return a > b ? a : b;
	}

	double AddSmooth::combine(double a, double b, Color colorIn, Color* colorOut) noexcept {
		double h = std::clamp(0.5 + 0.5 * (b - a) / m_smoothing, 0.0, 1.0);
		if (colorOut) blendColors(1.0 - h, colorIn, colorOut);
		return std::lerp(b, a, h) - m_smoothing * h * (1.0 - h);
		//a.y = 1.0 - h;
	}

	double SubtractSmooth::combine(double a, double b, Color colorIn, Color* colorOut) noexcept {
		double h = m_smoothing - abs(-b - a);
		h = h < 0.0 ? 0.0 : h;
		if (colorOut) *colorOut = colorIn;
		return (a > -b ? a : -b) + h * h * 0.25 / m_smoothing;
	}

	double IntersectSmooth::combine(double a, double b, Color colorIn, Color* colorOut) noexcept {
		double h = std::clamp(0.5 - 0.5 * (b - a) / m_smoothing, 0.0, 1.0);
		if (colorOut) blendColors(h, colorIn, colorOut);
		return std::lerp(b, a, h) + m_smoothing * h * (1.0 - h);
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
	void MirrorXYZ::repeat(Vec3& p) noexcept
	{
		p = p.abs();
	}
}