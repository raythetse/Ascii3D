#include "PixelSample.h"
#include <cmath>
#include "MathFunctions.h"

const std::string PixelSample::charGradient{ "-~+*vri325E8B@" };

void PixelSample::buildRenderParams(const SDFObject* object, Color color, Vec3 p, double maxDistance, const std::vector<std::unique_ptr<LightSource>>& lights, const std::vector<bool>& directLighting)
{
	Vec3 lightColor{ 0.0 };
	double totalLight{ 0.0 };
	double ambPower = 0.05;

	if (object) {
		for (size_t i{}; i < lights.size(); ++i) {
			LightSource* light = lights[i].get();

			Vec3 lightDir = (light->position() - samplePoint).getNormal();
			double lightDist = light->position().distance(samplePoint);

			double attenuation = 1.0 / (lightDist * lightDist);
			double directLight = directLighting[i] ? light->luminance() * attenuation : 0.0;
			double ambLight = light->luminance() * ambPower * attenuation;

			double dot = lightDir.dot(normal);
			dot = std::max(0.0, dot);
			directLight *= dot;

			double lightPower = directLight + ambLight;
			lightColor += light->color() * lightPower;
		}
		lightColor *= color;
		lightColor = lightColor.saturate();
		lightColor = lightColor.lerp(lightColor.getNormal(), 0.2);
		totalLight = std::min(1.0, lightColor.length());
	}

	//color = color.quantize(64);
	//totalLight = 1.0 - (camDistance / maxDistance);

	updateRenderChar(totalLight, camDistance / maxDistance);
	renderColor = camDistance >= maxDistance ? 0 : lightColor * 255;

	updateColorCode();
}

void PixelSample::updateColorCode()
{
	colorCode = std::format("\033[38;2;{};{};{}m",
		static_cast<int>(renderColor.x),
		static_cast<int>(renderColor.y),
		static_cast<int>(renderColor.z));
}

void PixelSample::updateRenderChar(double lightVal, double distanceRatio)
{
	size_t index = static_cast<size_t>(lightVal * (charGradient.length() - 1));
	renderChar = distanceRatio >= 1.0 ? ' ' : charGradient[index];
}
