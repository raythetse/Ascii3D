#pragma once
#include <chrono>

// Timer for use with performance monitor
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
	void reset();
	double tick(TimeUnit unit);
	double totalTime(TimeUnit unit) const;
	double totalMilli() const;
	double totalMicro() const;
	double totalNano() const;
};
