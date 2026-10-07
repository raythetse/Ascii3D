#pragma once
#include <ranges>
#include <vector>
#include "SDFObject.h"
#include "PixelSample.h"
#include "VulkanManager.h"

class PixelField
{
private:
	const size_t m_minWidth{ 15 };
	const size_t m_minHeight{ 5 };
	size_t m_width;
	size_t m_height;
	std::ranges::iota_view<size_t, size_t> m_pixelRange;
	std::vector<PixelSample> m_fieldA;
	std::vector<PixelSample> m_fieldB;
	bool m_swap{};
	void rebuildBuffers();
public:
	PixelField() = default;
	PixelSample& operator()(std::size_t row, std::size_t col);
	size_t width() const { return m_width; }
	size_t height() const { return m_height; }
	bool changedChar(std::size_t row, size_t col) const;
	bool changedColor(std::size_t row, size_t col) const;
	bool changed(std::size_t row, size_t col) const;
	const std::ranges::iota_view<size_t, size_t>& range();
	void swap();
	void setSize(const size_t width, const size_t height);
	void fromHardware(const PixelOutput* pixelBuffer);
};

