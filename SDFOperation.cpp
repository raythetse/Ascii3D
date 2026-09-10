#pragma once
#include "SDFOperation.h"

#include <cmath>
#include "GameTimer.h"
#include "SDFObject.h"
#include "Mat2.h"

void SDFOperation::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut)
{
	switch (opType())
	{
	case SDFOperationType::Combine:{
		distanceA = combine(p, distanceA, colorIn, colorOut);
		return;
	}
	case SDFOperationType::Resurface: {
		distanceA = resurface(distanceA);
		if (colorOut) *colorOut = colorIn;
		return;
	}
	case SDFOperationType::Transform: {
		transform(p);
		if (colorOut) *colorOut = colorIn;
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

void* CombinationOperation::getPropertyPtr(std::string_view propName)
{
	if (propName == "smoothing") return &m_smoothing;
	if (propName == "colorBlending") return &m_colorBlending;
	return nullptr;
}

double Add::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	if (colorOut) *colorOut = distanceA <= distanceB ? colorIn : *colorOut;
	return distanceA < distanceB ? distanceA : distanceB;
}

double Subtract::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	if (colorOut) *colorOut = colorIn;
	return distanceA > -distanceB ? distanceA : -distanceB;
}

double Intersect::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	if (colorOut) *colorOut = distanceA <= distanceB ? colorIn : *colorOut;
	return distanceA > distanceB ? distanceA : distanceB;
}

double AddSmooth::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	double h = std::clamp(0.5 + 0.5 * (distanceB - distanceA) / m_smoothing, 0.0, 1.0);
	if (colorOut) blendColors(1.0 - h, colorIn, colorOut);
	return std::lerp(distanceB, distanceA, h) - m_smoothing * h * (1.0 - h);
	//a.y = 1.0 - h;
}

double SubtractSmooth::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	double h = m_smoothing - abs(-distanceB - distanceA);
	h = h < 0.0 ? 0.0 : h;
	if (colorOut) *colorOut = colorIn;
	return (distanceA > -distanceB ? distanceA : -distanceB) + h * h * 0.25 / m_smoothing;
}

double IntersectSmooth::combine(Vec3 p, double distanceA, Color colorIn, Color* colorOut) {
	double distanceB = m_combineObj->signedDistance(p, colorOut);
	double h = std::clamp(0.5 - 0.5 * (distanceB - distanceA) / m_smoothing, 0.0, 1.0);
	if (colorOut) blendColors(h, colorIn, colorOut);
	return std::lerp(distanceB, distanceA, h) + m_smoothing * h * (1.0 - h);
}

void* ResurfaceOperation::getPropertyPtr(std::string_view propName)
{
	if (propName == "magnitude") return &m_magnitude;
	return nullptr;
}

double RoundOff::resurface(double distance)
{
	return distance - m_magnitude;
}

double Onion::resurface(double distance) {
	return abs(distance) - m_magnitude;
}

void MirrorX::transform(Vec3& p) const
{
	p.x = abs(p.x);
}

void MirrorY::transform(Vec3& p) const
{
	p.y = abs(p.x);
}

void MirrorZ::transform(Vec3& p) const
{
	p.z = abs(p.z);
}

void MirrorXY::transform(Vec3& p) const
{
	p.x = abs(p.x);
	p.y = abs(p.y);
}

void MirrorXZ::transform(Vec3& p) const
{
	p.x = abs(p.x);
	p.z = abs(p.z);
}

void MirrorYZ::transform(Vec3& p) const
{
	p.y = abs(p.y);
	p.z = abs(p.z);
}

void MirrorXYZ::transform(Vec3& p) const
{
	p = p.abs();
}

void* DomainOperation::getPropertyPtr(std::string_view propName)
{
	if (propName == "spacing") return &m_spacing;
	return nullptr;
}

void RepeatX::transform(Vec3& p) const
{
	if (!m_spacing.x) return;
	p.x -= m_spacing.x * round(p.x / m_spacing.x);
}

void RepeatY::transform(Vec3& p) const
{
	if (!m_spacing.y) return;
	p.y -= m_spacing.y * round(p.y / m_spacing.y);
}

void RepeatZ::transform(Vec3& p) const
{
	if (!m_spacing.z) return;
	p.z -= m_spacing.z * round(p.z / m_spacing.z);
}

void RepeatXY::transform(Vec3& p) const
{
	if (!m_spacing.x || !m_spacing.y) return;
	p.x -= m_spacing.x * round(p.x / m_spacing.x);
	p.y -= m_spacing.y * round(p.y / m_spacing.y);
}

void RepeatXZ::transform(Vec3& p) const
{
	if (!m_spacing.x || !m_spacing.z) return;
	p.x -= m_spacing.x * round(p.x / m_spacing.x);
	p.z -= m_spacing.z * round(p.z / m_spacing.z);
}

void RepeatYZ::transform(Vec3& p) const
{
	if (!m_spacing.y || !m_spacing.z) return;
	p.y -= m_spacing.y * round(p.y / m_spacing.y);
	p.z -= m_spacing.z * round(p.z / m_spacing.z);
}

void RepeatXYZ::transform(Vec3& p) const
{
	if (!m_spacing.x || !m_spacing.y || !m_spacing.z) return;
	p -= m_spacing * (p / m_spacing).round();
}

void RepeatLimited::transform(Vec3& p) const
{
	if (!m_spacing.x || !m_spacing.y || !m_spacing.z) return;
	p -= m_spacing * (p / m_spacing).round().clamp(-m_limits, m_limits);
}

void* RepeatLimited::getPropertyPtr(std::string_view propName)
{
	if (propName == "limits") return &m_limits;
	return nullptr;
}

void Translate::transform(Vec3& p) const
{
	p -= m_translation;
}

void* Translate::getPropertyPtr(std::string_view propName)
{
	if (propName == "translation") return &m_translation;
	return nullptr;
}

void TranslateSine::transform(Vec3& p) const
{
	p -= m_axis * m_amplitude * sin(GameTimer::gameTime() / m_period * MF::TAU + m_phase * MF::TAU);
}

void* TranslateSine::getPropertyPtr(std::string_view propName)
{
	if (propName == "amplitude") return &m_amplitude;
	if (propName == "axis") return &m_axis;
	if (propName == "period") return &m_period;
	if (propName == "phase") return &m_phase;
	return nullptr;
}

void TranslateLinear::transform(Vec3& p) const
{
	double time{ std::fmod(GameTimer::gameTime() + m_phase, m_cycleTime) };
	if (time <= m_transitTimes.x) {
		time /= m_transitTimes.x;
		p -= m_start.lerp(m_end, time);
		return;
	}
	double timeFrame = m_transitTimes.x + m_endDelays.x;
	if (time <= timeFrame) {
		p -= m_end;
		return;
	}
	timeFrame += m_transitTimes.y;
	if (time <= timeFrame) {
		time = (time - m_transitTimes.x - m_endDelays.x) / m_transitTimes.y;
		p -= m_end.lerp(m_start, time);
		return;
	}
	p -= m_start;
}

void* TranslateLinear::getPropertyPtr(std::string_view propName)
{
	if (propName == "cycleTime") return &m_cycleTime;
	if (propName == "end") return &m_end;
	if (propName == "endDelays") return &m_endDelays;
	if (propName == "phase") return &m_phase;
	if (propName == "start") return &m_start;
	if (propName == "transitTimes") return &m_transitTimes;
	return nullptr;
}

void CubeRollHeight::transform(Vec3& p) const
{
	double angle = std::fmod(GameTimer::gameTime() + m_phase, m_period) * MF::HALFPI - MF::HALFPI;
	Vec2 rot = Vec2{ -m_cubeHeight * 0.5, m_cubeHeight * 0.5 }.rotate(angle);
	p.y -= (rot.y - m_cubeHeight * 0.5);
}

void* CubeRollHeight::getPropertyPtr(std::string_view propName)
{
	if (propName == "cubeHeight") return &m_cubeHeight;
	if (propName == "period") return &m_period;
	if (propName == "phase") return &m_phase;
	return nullptr;
}

void Rotate::transform(Vec3& p) const
{
	p = m_rotMat * p;
}

void* Rotate::getPropertyPtr(std::string_view propName)
{
	if (propName == "axis") return &m_axis;
	if (propName == "rotations") return &m_rotations;
	if (propName == "rotMat") return &m_rotMat;
	return nullptr;
}

void RotateContinuous::transform(Vec3& p) const
{
	Mat3 m; Mat3::fromAxisAngle(m_axis, m, (GameTimer::gameTime() + (m_phase / m_rotationsPerSecond)) * m_rotationsPerSecond * MF::TAU);
	p = m * p;
}

void* RotateContinuous::getPropertyPtr(std::string_view propName)
{
	if (propName == "axis") return &m_axis;
	if (propName == "phase") return &m_phase;
	if (propName == "rotationsPerSecond") return &m_rotationsPerSecond;
	return nullptr;
}

void RotateScan::transform(Vec3& p) const
{
	double time{ std::fmod(GameTimer::gameTime() + m_phase, m_cycleTime) };
	if (time <= m_transitTimes.x) {
		time /= m_transitTimes.x;
		Mat3 m; Mat3::fromAxisAngle(m_axis, m, m_endAngle * time);
		p = m * p;
		return;
	}
	double timeFrame = m_transitTimes.x + m_endDelays.x;
	if (time <= timeFrame) {
		Mat3 m; Mat3::fromAxisAngle(m_axis, m, m_endAngle);
		p = m * p;
		return;
	}
	timeFrame += m_transitTimes.y;
	if (time <= timeFrame) {
		time = (time - (m_transitTimes.x + m_endDelays.x)) / m_transitTimes.y;
		Mat3 m; Mat3::fromAxisAngle(m_axis, m, m_endAngle * (1.0 - time));
		p = m * p;
		return;
	}
}

void* RotateScan::getPropertyPtr(std::string_view propName)
{
	if (propName == "axis") return &m_axis;
	if (propName == "cycleTime") return &m_cycleTime;
	if (propName == "endAngle") return &m_endAngle;
	if (propName == "endDelays") return &m_endDelays;
	if (propName == "phase") return &m_phase;
	if (propName == "transitTimes") return &m_transitTimes;
	return nullptr;
}

void CubeRoller::transform(Vec3& p) const
{
	double cycleTime = m_axes.size() * m_quarterTurnTime;
	double time = std::fmod(GameTimer::gameTime() + m_phase, cycleTime);
	size_t index = static_cast<size_t>(time);
	time /= m_quarterTurnTime;
	Mat3 m; Mat3::fromAxisAngle(m_axes[index], m, MF::HALFPI * time);
	p = m * p;
}

void* CubeRoller::getPropertyPtr(std::string_view propName)
{
	if (propName == "axes") return &m_axes;
	if (propName == "phase") return &m_phase;
	if (propName == "quarterTurnTime") return &m_quarterTurnTime;
	return nullptr;
}

void RotateDelay::transform(Vec3& p) const
{
	double completeCycles{ std::floor((GameTimer::gameTime() + m_phase) / m_cycleTime) };
	double time{ std::fmod(GameTimer::gameTime() + m_phase, m_cycleTime) };
	completeCycles += std::min(m_rotateTime, time) / m_rotateTime;
	Mat3 m; Mat3::fromAxisAngle(m_axis, m,completeCycles * m_angle);
	p = m * p;
}

void* RotateDelay::getPropertyPtr(std::string_view propName)
{
	if (propName == "angle") return &m_angle;
	if (propName == "axis") return &m_axis;
	if (propName == "cycleTime") return &m_cycleTime;
	if (propName == "phase") return &m_phase;
	if (propName == "rotateTime") return &m_rotateTime;
	return nullptr;
}

void Twist::transform(Vec3& p) const
{
	Vec2 sc = MF::fast_sincos(m_magnitude * p.y);
	Mat2  m = Mat2{ { sc.y, -sc.x, sc.x, sc.y} };
	Vec2 xz = m * Vec2{p.x, p.z };
	p = Vec3(xz.x, xz.y, p.y);
}

void Bend::transform(Vec3& p) const
{
	Vec2 sc = MF::fast_sincos(m_magnitude * p.x);
	Mat2  m = Mat2{ { sc.y, -sc.x, sc.x, sc.y } };
	Vec2 xy = m * Vec2{ p.x, p.y };
	p = Vec3(xy.x, xy.y, p.z);
}

void* DistortOperation::getPropertyPtr(std::string_view propName)
{
	if (propName == "magnitude") return &m_magnitude;
	return nullptr;
}
