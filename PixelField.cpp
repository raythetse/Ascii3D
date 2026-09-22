#include "PixelField.h"
#include <execution>
#include <ranges>
#include "ConsoleFunctions.h"

void PixelField::rebuildBuffers()
{
    m_fieldA = std::vector<PixelSample>(m_width * m_height);
    m_fieldB = std::vector<PixelSample>(m_width * m_height);
    m_pixelRange = std::views::iota(size_t(0), m_width * m_height);
    ConsoleFunctions::clearConsole();
}

PixelSample& PixelField::operator()(size_t row, size_t col)
{
    return m_swap ? m_fieldB[row * m_width + col] : m_fieldA[row * m_width + col];
}

bool PixelField::changedChar(std::size_t row, size_t col) const
{
    std::size_t index = row * m_width + col;
    return m_fieldB[index].renderChar != m_fieldA[index].renderChar;
}

bool PixelField::changedColor(std::size_t row, size_t col) const
{
    std::size_t index = row * m_width + col;
    return m_fieldB[index].renderColor != m_fieldA[index].renderColor;
}

bool PixelField::changed(std::size_t row, size_t col) const
{
    std::size_t index = row * m_width + col;
    return changedColor(row, col) || changedChar(row, col);
}

const std::ranges::iota_view<size_t, size_t>& PixelField::range()
{
    return m_pixelRange;
}

void PixelField::swap()
{
    m_swap = !m_swap;
}

void PixelField::setSize(const size_t width, const size_t height)
{
    if (m_width != width || m_height != height) {
        m_width = width;
        m_height = height;
        rebuildBuffers();
    }
}

void PixelField::fromHardware(const PixelOutput* pixelBuffer)
{
    auto& field = m_swap ? m_fieldB : m_fieldA;

    std::for_each(std::execution::par, m_pixelRange.begin(), m_pixelRange.end(), [this, &field, pixelBuffer](size_t index) {
        size_t row = index / m_width;
        size_t col = index % m_width;

        field[index].renderColor.x = pixelBuffer[index].r * 255.0;
        field[index].renderColor.y = pixelBuffer[index].g * 255.0;
        field[index].renderColor.z = pixelBuffer[index].b * 255.0;
        field[index].camDistance = 1.0 - pixelBuffer[index].l;

        field[index].updateRenderChar(field[index].camDistance, field[index].camDistance);
        field[index].updateColorCode();
    });
}
