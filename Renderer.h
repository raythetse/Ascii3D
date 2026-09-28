#pragma once
#include <ranges>
#include <vector>
#include <string>
#include "PixelField.h"
class Renderer
{
private:
	std::string m_frameBuffer{};
	size_t m_width{};
	size_t m_height{};
	std::vector<PixelSample> m_prevFrame;
public:
	void render(PixelField& field);
};

