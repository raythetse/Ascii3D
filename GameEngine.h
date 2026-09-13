#pragma once
#include <utility>
#include "PixelField.h"
#include "PerfMonitor.h"
#include "RayMarcher.h"
#include "Renderer.h"
#include "SDFObject.h"
#include "Stopwatch.h"
#include "SDFScene.h"

class GameEngine
{
private:
	std::pair<size_t, size_t> m_dimensions;
	double m_sceneDepth;
	PixelField m_pixelField;
	RayMarcher m_rayMarcher;
	Renderer m_renderer{};
	Camera m_camera;
	SDFScene m_scene;
	bool m_pausePressed{};
	PerfMonitor m_perfMonitor{};
	size_t m_framePM{};
	size_t m_updatePM{};
	size_t m_marchPM{};
	size_t m_renderPM{};

	void resizeCallback(size_t width, size_t height);

public:
	GameEngine(size_t width, size_t height, double maxSceneDepth, double stepFactor, double tolerance);
	void run();
	bool handleInput();
	void update();
	void march();
	void render();
};

