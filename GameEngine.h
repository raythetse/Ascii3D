#pragma once
#include <utility>
#include "PixelField.h"
#include "PerfMonitor.h"
#include "RayMarcher.h"
#include "Renderer.h"
#include "SDFObject.h"
#include "Stopwatch.h"
#include "SDFScene.h"
#include "VulkanRaymarcher.h"

class GameEngine
{
	enum MarchingMode {
		Software,
		Hardware,
	};
private:
	std::pair<size_t, size_t> m_dimensions{ 120, 40 };
	double m_sceneDepth;
	double m_surTolerance;
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
	VulkanRaymarcher m_vrm{};
	MarchingMode m_marchMode{ Software };
	PixelOutput* m_hardwareRenderResult = nullptr;

	void resizeCallback(size_t width, size_t height);
	void marchHardware();

public:
	GameEngine(size_t width, size_t height, double maxSceneDepth, double tolerance);
	void run();
	bool handleInput();
	void update();
	void march();
	void render();
};

