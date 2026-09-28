#include "Renderer.h"
#include <Windows.h>
#include <iostream>
#include <array>
#include <charconv>

// 1. Define a global or static 15-bit color table (32 * 32 * 32 = 32,768 colors)
struct AnsiColorCode {
    char data[24];
    uint8_t len;
};

class ColorLUT {
public:
    std::array<AnsiColorCode, 32768> table;

    ColorLUT() {
        // Pre-calculate every single color string once at startup
        for (int r = 0; r < 32; ++r) {
            for (int g = 0; g < 32; ++g) {
                for (int b = 0; b < 32; ++b) {
                    // Scale 0-31 up to 0-255
                    int realR = (r * 255) / 31;
                    int realG = (g * 255) / 31;
                    int realB = (b * 255) / 31;

                    char* ptr = table[(r << 10) | (g << 5) | b].data;
                    std::memcpy(ptr, "\033[38;2;", 7);
                    ptr += 7;

                    auto res = std::to_chars(ptr, ptr + 4, realR); ptr = res.ptr; *ptr++ = ';';
                    res = std::to_chars(ptr, ptr + 4, realG);     ptr = res.ptr; *ptr++ = ';';
                    res = std::to_chars(ptr, ptr + 4, realB);     ptr = res.ptr; *ptr++ = 'm';

                    table[(r << 10) | (g << 5) | b].len = static_cast<uint8_t>(ptr - table[(r << 10) | (g << 5) | b].data);
                }
            }
        }
    }
};

// Instantiate the table once globally
static ColorLUT g_ColorLUT;

void Renderer::render(PixelField& field)
{
    const size_t maxFrameSize = (field.width() * field.height() * 24) + 32;

    if (m_width != field.width() || m_height != field.height()) {
        m_width = field.width();
        m_height = field.height();
        m_frameBuffer.resize(maxFrameSize);
    }

    char* out = m_frameBuffer.data();

    // Reset terminal cursor position
    std::memcpy(out, "\033[1;1H", 6);
    out += 6;

    uint16_t lastColorKey = 0xFFFF; // Out of bounds initial state

    for (int row = 0; row < field.height(); ++row) {
        for (int col = 0; col < field.width(); ++col) {
            auto& pixel = field(row, col);

            // Quantize to 0-31 range 
            int r = static_cast<int>(pixel.renderColor.x) >> 3;
            int g = static_cast<int>(pixel.renderColor.y) >> 3;
            int b = static_cast<int>(pixel.renderColor.z) >> 3;

            // Generate 15-bit key
            uint16_t colorKey = (r << 10) | (g << 5) | b;

            // Update if the key changed
            if (colorKey != lastColorKey) {
                const auto& ansi = g_ColorLUT.table[colorKey];
                std::memcpy(out, ansi.data, ansi.len);
                out += ansi.len;
                lastColorKey = colorKey;
            }

            *out++ = pixel.renderChar;
        }
        *out++ = '\n';
    }

    // Write straight terminal window
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;
    size_t totalBytes = out - m_frameBuffer.data();
    WriteConsoleA(hConsole, m_frameBuffer.data(), static_cast<DWORD>(totalBytes), &written, NULL);
}