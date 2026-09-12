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
	static GameTimer& instance();
	void updateTick();

public:
	GameTimer(const GameTimer&) = delete;
	GameTimer& operator=(const GameTimer&) = delete;
	GameTimer(GameTimer&&) = delete;
	GameTimer& operator=(GameTimer&&) = delete;
	static void togglePause();
	static void tick();
	static double delta();
	static double appTime();
	static double gameTime();
	static bool isPaused();
	static void reset();
};

