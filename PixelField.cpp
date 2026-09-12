#include "PixelField.h"

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
