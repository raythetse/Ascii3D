#pragma once
#include <chrono>
class Stopwatch
{
private:
	using Clock = std::chrono::steady_clock;
	using DurSecond = std::chrono::duration<double, std::ratio<1, 1>>;
	using DurMilli = std::chrono::duration<double, std::milli>;
	using DurMicro = std::chrono::duration<double, std::micro>;
	using DurNano = std::chrono::duration<double, std::nano>;

	std::chrono::steady_clock::time_point m_startTime;
	std::chrono::steady_clock::time_point m_lastTime;

public:
	enum TimeUnit {
		Second,
		Milli,
		Micro,
		Nano,
	};

	Stopwatch() { reset(); }

	void reset()
	{
		m_startTime = Clock::now();
		m_lastTime = Clock::now();
	}

	double tick(TimeUnit unit) 
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

	double totalTime(TimeUnit unit) const {
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

	double totalMilli() const {
		auto currentTime = std::chrono::steady_clock::now();
		std::chrono::duration<float> totalTime = currentTime - m_startTime;
		return std::chrono::duration_cast<DurMilli>(totalTime).count();
	}

	double totalMicro() const {
		auto currentTime = std::chrono::steady_clock::now();
		std::chrono::duration<float> totalTime = currentTime - m_startTime;
		return std::chrono::duration_cast<DurMicro>(totalTime).count();
	}

	double totalNano() const {
		auto currentTime = std::chrono::steady_clock::now();
		std::chrono::duration<float> totalTime = currentTime - m_startTime;
		return std::chrono::duration_cast<DurNano>(totalTime).count();
	}
};
