#include "PixelSample.h"
#include <cmath>

const std::string PixelSample::charGradient{ ".~<=vri325EB" };

void PixelSample::buildRenderParams(const std::unique_ptr<SDFObject>& object, Vec3 p, double maxDistance, const std::vector<LightSource*>& lights)
{
	double totalLight{};

	for (auto& l : lights) {
		Vec3 lightPos = l->position();
		Vec3 lightDir = (l->position() - samplePoint).getNormal();
		double lightDist = lightPos.distance(samplePoint);
		double light = l->luminance() / (4.0 * 3.14159265359 * lightDist * lightDist);
		//light = light > 1.0 ? 1.0 : light;

		double dot = lightDir.dot(normal);
		dot = dot < 0.0 ? 0.0 : dot;
		light *= dot;
		totalLight += light;
	}

	totalLight = totalLight > 1.0 ? 1.0 : totalLight;

	Color color{};
	if (object) {
		object->signedDistance(p, &color);
		color = color.quantize(16);
		//totalLight *= 1.0 - (camDistance / maxDistance);
	}

	size_t index = static_cast<size_t>(totalLight * (charGradient.length() - 1));
	renderChar = camDistance >= maxDistance ? ' ' : charGradient[index];
	renderColor = camDistance >= maxDistance ? 0 : color * 255;

	auto r = std::to_string(static_cast<int>(renderColor.x));
	auto g = std::to_string(static_cast<int>(renderColor.y));
	auto b = std::to_string(static_cast<int>(renderColor.z));
	colorCode = "\033[38;2;" + r + ";" + g + ";" + b + "m";
}