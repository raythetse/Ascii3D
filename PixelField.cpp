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

std::vector<PixelSample>::const_iterator PixelField::fieldBegin() const
{
    return m_swap ? m_fieldB.begin() : m_fieldA.begin();
}

std::vector<PixelSample>::const_iterator PixelField::fieldEnd() const
{
    return m_swap ? m_fieldB.end() : m_fieldA.end();
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

std::ranges::iterator_t<std::ranges::iota_view<size_t, size_t>> PixelField::rangeBegin()
{
    return m_pixelRange.begin();
}

std::ranges::iterator_t<std::ranges::iota_view<size_t, size_t>> PixelField::rangeEnd()
{
    return m_pixelRange.end();
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
    m_width = 5 > m_width - decrease ? 5 : m_width - decrease;
    rebuildBuffers();
    return { static_cast<double>(m_width), static_cast<double>(m_height) };
}

Vec2 PixelField::decreaseHeight(size_t decrease)
{
    m_height = 5 > m_height - decrease ? 5 : m_height - decrease;
    rebuildBuffers();
    return { static_cast<double>(m_width), static_cast<double>(m_height) };
}
