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
    std::for_each(std::execution::par, m_pixelRange.begin(), m_pixelRange.end(), [this, pixelBuffer](size_t index) {
        size_t row = index / m_width;
        size_t col = index % m_width;

        (*this)(row, col).renderColor.x = pixelBuffer[index].r;
        (*this)(row, col).renderColor.y = pixelBuffer[index].g;
        (*this)(row, col).renderColor.z = pixelBuffer[index].b;
        (*this)(row, col).renderColor = (*this)(row, col).renderColor.quantize(16);
        (*this)(row, col).renderColor *= 255.0;

        (*this)(row, col).updateRenderChar(pixelBuffer[index].l, 1.0 - pixelBuffer[index].d);
        (*this)(row, col).updateColorCode();
    });
}
