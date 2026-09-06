#pragma once
#include <vector>
#include "SceneObject.h"
#include "SceneSample.h"

class PixelField
{
private:
	size_t m_width;
	size_t m_height;
	std::vector<SceneSample> m_fieldA;
	std::vector<SceneSample> m_fieldB;
	bool m_swap{};
public:
	PixelField(const size_t width, const size_t height) : m_width(width), m_height(height), m_fieldA(std::vector<SceneSample>(width* height)), m_fieldB(std::vector<SceneSample>(width* height)) {}
	SceneSample& operator()(std::size_t row, std::size_t col);
	size_t width() const { return m_width; }
	size_t height() const { return m_height; }
	std::vector<SceneSample>::const_iterator begin() const;
	std::vector<SceneSample>::const_iterator end() const;
	bool changedChar(std::size_t row, size_t col) const;
	bool changedColor(std::size_t row, size_t col) const;
	bool changed(std::size_t row, size_t col) const;
	void swap();
};

