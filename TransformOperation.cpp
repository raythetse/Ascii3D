#include "TransformOperation.h"
#include "GameTimer.h"

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
	Mat3 m; Mat3::fromAxisAngle(m_axis, m, completeCycles * m_angle);
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