#pragma once
#include <string>
#include "PixelField.h"
class Renderer
{
private:
	bool firstRun{ true };
	std::string m_frameBuffer{};
public:
	Renderer(size_t numPixels);
	void render(PixelField& field);
};

