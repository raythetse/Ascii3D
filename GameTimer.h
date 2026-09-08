#pragma once
#include "Stopwatch.h"

class GameTimer
{
private:
	Stopwatch m_stopwatch{};
	double m_delta{};
	GameTimer() {}
	~GameTimer() {}
	
	static GameTimer& instance() {
		static GameTimer instance;
		return instance;
	}
	void UpdateTick() {
		m_delta = m_stopwatch.tick(Stopwatch::Second);
	}

public:
	GameTimer(const GameTimer&) = delete;
	GameTimer& operator=(const GameTimer&) = delete;
	GameTimer(GameTimer&&) = delete;
	GameTimer& operator=(GameTimer&&) = delete;

	static void Tick() {
		instance().UpdateTick();
	}

	static double Delta() {
		return instance().m_delta;
	}

	static void reset() {
		instance().m_stopwatch.reset();
	}

	static double totalTime() {
		return instance().m_stopwatch.totalTime(Stopwatch::Second);
	}
};

