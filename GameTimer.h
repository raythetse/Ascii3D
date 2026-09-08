#pragma once
#include "Stopwatch.h"

class GameTimer
{
private:
	Stopwatch m_stopwatch{};
	double m_deltaTime{};
	double m_appTime{};
	double m_gameTime{};
	bool m_isPaused{};
	GameTimer() {}
	~GameTimer() {}
	
	static GameTimer& instance() {
		static GameTimer instance;
		return instance;
	}
	void updateTick() {
		m_deltaTime = m_stopwatch.tick(Stopwatch::Second);
		m_appTime += m_deltaTime;
		if (!m_isPaused) m_gameTime += m_deltaTime;
	}

public:
	GameTimer(const GameTimer&) = delete;
	GameTimer& operator=(const GameTimer&) = delete;
	GameTimer(GameTimer&&) = delete;
	GameTimer& operator=(GameTimer&&) = delete;

	static void togglePause() {
		instance().m_isPaused = !instance().m_isPaused;
	}

	static void tick() {
		instance().updateTick();
	}

	static double delta() {
		return instance().m_deltaTime;
	}

	static double appTime() {
		return instance().m_appTime;
	}

	static double gameTime() {
		return instance().m_gameTime;
	}

	static bool isPaused() {
		return instance().m_isPaused;
	}

	static void reset() {
		instance().m_stopwatch.reset();
		instance().m_deltaTime = 0.0;
		instance().m_gameTime = 0.0;
		instance().m_isPaused = false;
	}
};

