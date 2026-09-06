#pragma once
#include <utility>
#include "PixelField.h"
#include "RayMarcher.h"
#include "Renderer.h"
#include "SceneObject.h"
#include "Stopwatch.h"

class GameEngine
{
private:
	std::pair<size_t, size_t> m_dimensions;
	double m_sceneDepth;
	PixelField m_pixelField;
	RayMarcher m_rayMarcher;
	Renderer m_renderer;
	Camera m_camera;
	std::vector<std::unique_ptr<SceneObject>> m_objects{};
	std::vector<std::unique_ptr<LightSource>> m_lights{};
	std::ranges::iota_view<size_t, size_t> m_pixelRange;
	std::ranges::iota_view<size_t, size_t> m_objectRange;
	Stopwatch m_gameTimer{};
	double m_deltaTime{};
	double m_time{};

public:
	GameEngine(size_t width, size_t height, double maxSceneDepth, double tolerance);
	void update();
	void march();
	void render();
};

