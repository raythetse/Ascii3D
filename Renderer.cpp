#include "Renderer.h"
#include <execution>
#include <iostream>

void Renderer::render(PixelField& field)
{
	if (m_width != field.width() || m_height != field.height()) {
		m_width = field.width();
		m_height = field.height();
		m_rowBuffer.clear();
		m_rowBuffer.reserve(m_height);
		for (size_t i{}; i < m_height; ++i) {
			m_rowBuffer.push_back("");
			m_rowBuffer[i].reserve(field.width() * 40);
		}
		m_rowIndices.resize(field.height());
		std::iota(m_rowIndices.begin(), m_rowIndices.end(), 0);
		m_frameBuffer.reserve(m_width * m_height * 40);
	}
	else {
		for (auto& str : m_rowBuffer) {
			str.clear();
		}
	}

	size_t numChanges{};

	std::for_each(std::execution::par, m_rowIndices.begin(), m_rowIndices.end(), [this, &field, &numChanges](size_t row) {
		Vec3 pColor{ -1, -1, -1 };
		for (int col{}; col < field.width(); ++col) {
			Vec3 color = field(row, col).renderColor;
			if (field.changedColor(row, col) || color != pColor) {
				m_rowBuffer[row] += field(row, col).colorCode;
				++numChanges;
			}
			m_rowBuffer[row] += field(row, col).renderChar;
			pColor = color;
		}
		m_rowBuffer[row] += '\n';
	});

	m_frameBuffer.clear();
	m_frameBuffer += "\033[1;1H"; // Set cursor to top left

	for (auto& str : m_rowBuffer) {
		m_frameBuffer += str;
	}

	std::cout.write(m_frameBuffer.data(), m_frameBuffer.size());
	std::cout.flush();
}
