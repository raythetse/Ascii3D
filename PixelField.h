#pragma once
#include <ranges>
#include <vector>
#include "SDFObject.h"
#include "PixelSample.h"

class PixelField
{
private:
	size_t m_width;
	size_t m_height;
	std::ranges::iota_view<size_t, size_t> m_pixelRange;
	std::vector<PixelSample> m_fieldA;
	std::vector<PixelSample> m_fieldB;
	bool m_swap{};
	void rebuildBuffers();
public:
	PixelField(const size_t width, const size_t height)
		: m_width(width), m_height(height)
		, m_fieldA(std::vector<PixelSample>(width * height))
		, m_fieldB(std::vector<PixelSample>(width * height))
		, m_pixelRange{ std::views::iota(size_t(0), width * height) } {}
	PixelSample& operator()(std::size_t row, std::size_t col);
	size_t width() const { return m_width; }
	size_t height() const { return m_height; }
	std::vector<PixelSample>::const_iterator fieldBegin() const;
	std::vector<PixelSample>::const_iterator fieldEnd() const;
	bool changedChar(std::size_t row, size_t col) const;
	bool changedColor(std::size_t row, size_t col) const;
	bool changed(std::size_t row, size_t col) const;
	std::ranges::iterator_t<std::ranges::iota_view<size_t, size_t>> rangeBegin();
	std::ranges::iterator_t<std::ranges::iota_view<size_t, size_t>> rangeEnd();
	void swap();
	Vec2 increaseWidth(size_t increase);
	Vec2 increaseHeight(size_t increase);
	Vec2 decreaseWidth(size_t decrease);
	Vec2 decreaseHeight(size_t decrease);
};

