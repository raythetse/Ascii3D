#pragma once
#include <ranges>
#include <vector>
#include <string>
#include "PixelField.h"
class Renderer
{
private:
	std::vector<std::string> m_rowBuffer{};
	std::string m_frameBuffer{};
	size_t m_width{};
	size_t m_height{};
	std::vector<size_t> m_rowIndices{};
public:
	void render(PixelField& field);
};

