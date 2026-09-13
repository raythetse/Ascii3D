#pragma once
#include <string>
#include "PixelField.h"
class Renderer
{
private:
	std::string m_frameBuffer{};
public:
	void render(PixelField& field);
};

