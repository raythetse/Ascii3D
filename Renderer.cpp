#include <iostream>
#include "Renderer.h"

void Renderer::render(PixelField& field)
{
	m_frameBuffer.clear();
	size_t fieldSize = field.width() * field.height() * 40;
	if (m_frameBuffer.capacity() < fieldSize) {
		m_frameBuffer.reserve(fieldSize);
	}
	m_frameBuffer += "\033[1;1H"; // Set cursor to top left
	Vec3 pColor{-1, -1, -1};

	for (int row{}; row < field.height(); ++row) {
		for (int col{}; col < field.width(); ++col) {
			Vec3 color = field(row, col).renderColor;
			if (field.changedColor(row, col) || color != pColor) {
				m_frameBuffer += field(row, col).colorCode;
			}
			m_frameBuffer += field(row, col).renderChar;
			pColor = color;
		}
		m_frameBuffer += '\n';
	}
	std::cout.write(m_frameBuffer.data(), m_frameBuffer.size());
	std::cout.flush();
}
