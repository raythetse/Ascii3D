#pragma once
#include "Operation.h"
#include "SceneObject.h"
#include "Mat2.h"

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
		case SDFOperationType::Mirror: {
			MirrorOperation* mirpOp = dynamic_cast<MirrorOperation*>(this);
			mirpOp->reflect(p);
			return;
		}
		case SDFOperationType::Domain: {
			DomainOperation* domOp = dynamic_cast<DomainOperation*>(this);
			domOp->repeat(p);
			return;
		}
		case SDFOperationType::Transform: {
			TransformOperation* TransOp = dynamic_cast<TransformOperation*>(this);
			TransOp->transform(p);
			return;
		}
		case SDFOperationType::Distort: {
			DistortOperation* DistOp = dynamic_cast<DistortOperation*>(this);
			DistOp->distort(p);
			return;
		}
		default:
			break;
		}
	}
	
	void CombinationOperation::blendColors(double alpha, Vec3 colorIn, Vec3* colorOut)
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

	double RoundOff::resurface(double distance)
	{
		return distance - m_magnitude;
	}

	double Onion::resurface(double distance) {
		return abs(distance) - m_magnitude;
	}

	void MirrorX::reflect(Vec3& p) noexcept
	{
		p.x = abs(p.x);
	}

	void MirrorY::reflect(Vec3& p) noexcept
	{
		p.y = abs(p.x);
	}

	void MirrorZ::reflect(Vec3& p) noexcept
	{
		p.z = abs(p.z);
	}

	void MirrorXY::reflect(Vec3& p) noexcept
	{
		p.x = abs(p.x);
		p.y = abs(p.y);
	}

	void MirrorXZ::reflect(Vec3& p) noexcept
	{
		p.x = abs(p.x);
		p.z = abs(p.z);
	}

	void MirrorYZ::reflect(Vec3& p) noexcept
	{
		p.y = abs(p.y);
		p.z = abs(p.z);
	}

	void MirrorXYZ::reflect(Vec3& p) noexcept
	{
		p = p.abs();
	}

	void RepeatX::repeat(Vec3& p)
	{
		if (!m_spacing.x) return;
		p.x -= m_spacing.x * round(p.x / m_spacing.x);
	}

	void RepeatY::repeat(Vec3& p)
	{
		if (!m_spacing.y) return;
		p.y -= m_spacing.y * round(p.y / m_spacing.y);
	}

	void RepeatZ::repeat(Vec3& p)
	{
		if (!m_spacing.z) return;
		p.z -= m_spacing.z * round(p.z / m_spacing.z);
	}

	void RepeatXY::repeat(Vec3& p)
	{
		if (!m_spacing.x || !m_spacing.y) return;
		p.x -= m_spacing.x * round(p.x / m_spacing.x);
		p.y -= m_spacing.y * round(p.y / m_spacing.y);
	}

	void RepeatXZ::repeat(Vec3& p)
	{
		if (!m_spacing.x || !m_spacing.z) return;
		p.x -= m_spacing.x * round(p.x / m_spacing.x);
		p.z -= m_spacing.z * round(p.z / m_spacing.z);
	}

	void RepeatYZ::repeat(Vec3& p)
	{
		if (!m_spacing.y || !m_spacing.z) return;
		p.y -= m_spacing.y * round(p.y / m_spacing.y);
		p.z -= m_spacing.z * round(p.z / m_spacing.z);
	}

	void RepeatXYZ::repeat(Vec3& p)
	{
		if (!m_spacing.x || !m_spacing.y || !m_spacing.z) return;
		p -= m_spacing * (p / m_spacing).round();
	}

	void RepeatLimited::repeat(Vec3& p)
	{
		if (!m_spacing.x || !m_spacing.y || !m_spacing.z) return;
		p -= m_spacing * (p / m_spacing).round().clamp(-m_limits, m_limits);
	}

	void Translate::transform(Vec3& p)
	{
		p -= m_transposition;
	}

	void Rotate::transform(Vec3& p)
	{
		p = m_rotMat * p;
	}

	void Twist::distort(Vec3& p)
	{
		Vec2 sc = MF::fast_sincos(m_magnitude * p.y);
		Mat2  m = Mat2{ { sc.y, -sc.x, sc.x, sc.y} };
		Vec2 xz = m * Vec2{p.x, p.z };
		p = Vec3(xz.x, xz.y, p.y);
	}

	void Bend::distort(Vec3& p)
	{
		Vec2 sc = MF::fast_sincos(m_magnitude * p.x);
		Mat2  m = Mat2{ { sc.y, -sc.x, sc.x, sc.y } };
		Vec2 xy = m * Vec2{ p.x, p.y };
		p = Vec3(xy.x, xy.y, p.z);
	}
}