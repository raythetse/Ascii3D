#include "Stopwatch.h"

void Stopwatch::reset()
{
	m_startTime = Clock::now();
	m_lastTime = Clock::now();
}

double Stopwatch::tick(TimeUnit unit)
{
	auto currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<float> elapsed = currentTime - m_lastTime;
	m_lastTime = currentTime;

	switch (unit)
	{
	case Stopwatch::Milli:
		return std::chrono::duration_cast<DurMilli>(elapsed).count();
	case Stopwatch::Micro:
		return std::chrono::duration_cast<DurMicro>(elapsed).count();
	case Stopwatch::Nano:
		return std::chrono::duration_cast<DurNano>(elapsed).count();
	default:
		return std::chrono::duration_cast<DurSecond>(elapsed).count();
	}
}

double Stopwatch::totalTime(TimeUnit unit) const
{
	auto currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<float> totalTime = currentTime - m_startTime;

	switch (unit)
	{
	case Stopwatch::Milli:
		return std::chrono::duration_cast<DurMilli>(totalTime).count();
	case Stopwatch::Micro:
		return std::chrono::duration_cast<DurMicro>(totalTime).count();
	case Stopwatch::Nano:
		return std::chrono::duration_cast<DurNano>(totalTime).count();
	default:
		return std::chrono::duration_cast<DurSecond>(totalTime).count();
	}
}

double Stopwatch::totalMilli() const
{
	auto currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<float> totalTime = currentTime - m_startTime;
	return std::chrono::duration_cast<DurMilli>(totalTime).count();
}

double Stopwatch::totalMicro() const
{
	auto currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<float> totalTime = currentTime - m_startTime;
	return std::chrono::duration_cast<DurMicro>(totalTime).count();
}

double Stopwatch::totalNano() const
{
	auto currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<float> totalTime = currentTime - m_startTime;
	return std::chrono::duration_cast<DurNano>(totalTime).count();
}
