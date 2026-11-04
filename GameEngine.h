#pragma once
#include <string>
#include <utility>
#include "Enumerations.h"
#include "PixelField.h"
#include "PerfMonitor.h"
#include "RayMarcher.h"
#include "Renderer.h"
#include "SDFObjectInc.h"
#include "Stopwatch.h"
#include "SDFScene.h"
#include "VulkanManager.h"

// Manages initialization of scenes and the marching/rendering loops
class GameEngine
{
private:
	std::pair<size_t, size_t> m_dimensions{ 120, 40 };
	Vec2 m_viewportSize{ 2.0, 1.0 };
	double m_sceneDepth;
	double m_surTolerance;
	PixelField m_pixelField;
	RayMarcher m_rayMarcher;
	Renderer m_renderer{};
	Camera m_camera;
	std::unique_ptr<SDFScene> m_scene;
	bool m_pausePressed{};
	PerfMonitor m_perfMonitor{};
	size_t m_framePM{};
	size_t m_updatePM{};
	size_t m_marchPM{};
	size_t m_renderPM{};
	std::unique_ptr<VulkanManager> m_vulkanManager;
	MarchingMode m_marchMode{ MARCHINGMODE_SOFTWARE };
	PixelOutput* m_hardwareRenderResult = nullptr;

	void resizeCallback(size_t width, size_t height);
	bool selectRenderer();
	bool selectSceneType();
	void selectBuiltInScene(bool noCustom);
	bool selectCustomScene();
	bool confirmLoadWarnings(const std::string& sceneName);
	void marchHardware();
	bool handleInput();
	void update();
	void march();
	void render();

public:
	GameEngine(double maxSceneDepth, double tolerance);
	void run();
};

