#include "PixelField.h"
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

const std::ranges::iota_view<size_t, size_t> PixelField::range()
{
    return m_pixelRange;
}

void PixelField::swap()
{
    m_swap = !m_swap;
}

Vec2 PixelField::increaseWidth(size_t increase)
{
    m_width += increase;
    rebuildBuffers();
    return { static_cast<double>(m_width), static_cast<double>(m_height) };
}

Vec2 PixelField::increaseHeight(size_t increase)
{
    m_height += increase;
    rebuildBuffers();
    return { static_cast<double>(m_width), static_cast<double>(m_height) };
}

Vec2 PixelField::decreaseWidth(size_t decrease)
{
    m_width -= min(decrease, m_width - m_minWidth);
    rebuildBuffers();
    return { static_cast<double>(m_width), static_cast<double>(m_height) };
}

Vec2 PixelField::decreaseHeight(size_t decrease)
{
    m_height -= min(decrease, m_height - m_minHeight);
    rebuildBuffers();
    return { static_cast<double>(m_width), static_cast<double>(m_height) };
}

void PixelField::setSize(size_t width, size_t height)
{
    if (m_width != width || m_height != height) {
        m_width = width;
        m_height = height;
        rebuildBuffers();
    }
}
