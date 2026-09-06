#pragma once
#include "Operation.h"
#include "SceneObject.h"

void Operation::execute(double& distance, Vec3 p, double time, const Vec3 colorIn, Vec3* colorOut)
{
	switch (m_opType)
	{
	case Operation::Combination: {
		Vec2 v{ distance, 0.0 };
		OpCombine::CombineOperation* comOp = dynamic_cast<OpCombine::CombineOperation*>(this);
		double distanceb = comOp->m_combineObj->signedDistance(p, time, colorOut);
		comOp->combine(v, distanceb);
		distance = v.x;
		if (colorOut) {
			double blendRemap = comOp->m_colorBlending;
			v.y *= blendRemap;
			v.y -= 0.5 * (blendRemap - 1.0);
			v.y = v.y < 0.0 ? 0.0 : v.y;
			v.y = v.y > 1.0 ? 1.0 : v.y;
			*colorOut = colorIn.lerp(*colorOut, comOp->m_colorBlendBias * v.y);
			return;
		}
		break;
	}
	case Operation::Surface: {
		OpSurface::SurfaceOperation* surfOp = dynamic_cast<OpSurface::SurfaceOperation*>(this);
		distance = surfOp->resurface(distance);
		break;
	}
	case Operation::Spacial:
		break;
	case Operation::Transform:
		break;
	default:
		break;
	}
		

	if (colorOut) {
		*colorOut = colorIn;
	}
}

namespace OpCombine
{
	void Join::combine(Vec2& a, double b) noexcept {
		a.x = a.x < b ? a.x : b;
		a.y = 0.0;
	}

	void Subtract::combine(Vec2& a, double b) noexcept {
		a.x = a.x > -b ? a.x : -b;
		a.y = 0.0;
	}

	void Intersect::combine(Vec2& a, double b) noexcept {
		a.x = a.x > b ? a.x : b;
		a.y = 0.0;
	}

	void JoinSmooth::combine(Vec2& a, double b) noexcept {
		double h = std::clamp(0.5 + 0.5 * (b - a.x) / m_smoothing, 0.0, 1.0);
		a.x = std::lerp(b, a.x, h) - m_smoothing * h * (1.0 - h);
		a.y = 1.0 - h;
	}

	void SubtractSmooth::combine(Vec2& a, double b) noexcept {
		double h = m_smoothing - abs(-b - a.x);
		h = h < 0.0 ? 0.0 : h;
		a.x = (a.x > -b ? a.x : -b) + h * h * 0.25 / m_smoothing;
	}

	void IntersectSmooth::combine(Vec2& a, double b) noexcept {
		double h = std::clamp(0.5 - 0.5 * (b - a.x) / m_smoothing, 0.0, 1.0);
		a.x = std::lerp(b, a.x, h) + m_smoothing * h * (1.0 - h);
		a.y = 1.0 - h;
	}
}

namespace OpSurface
{
	double Onion::resurface(double distance)	{
		return abs(distance) - m_magnitude;
	}	
}
