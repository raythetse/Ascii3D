#include "GameTimer.h"

GameTimer& GameTimer::instance()
{
	static GameTimer instance;
	return instance;
}

void GameTimer::updateTick()
{
	m_deltaTime = m_stopwatch.tick(Stopwatch::Second);
	m_appTime += m_deltaTime;
	if (!m_isPaused) m_gameTime += m_deltaTime;
}

void GameTimer::togglePause()
{
	instance().m_isPaused = !instance().m_isPaused;
}

void GameTimer::tick()
{
	instance().updateTick();
}

double GameTimer::delta()
{
	return instance().m_deltaTime;
}

double GameTimer::appTime()
{
	return instance().m_appTime;
}

double GameTimer::gameTime()
{
	return instance().m_gameTime;
}

bool GameTimer::isPaused()
{
	return instance().m_isPaused;
}

void GameTimer::reset(bool paused)
{
	instance().m_stopwatch.reset();
	instance().m_deltaTime = 0.0;
	instance().m_gameTime = 0.0;
	instance().m_isPaused = paused;
}
