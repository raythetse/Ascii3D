#include "PixelSample.h"
#include <cmath>
#include "MathFunctions.h"

const std::string PixelSample::s_charGradient{ " -~+*vri325E8B@" };

void PixelSample::buildRenderParams(const SDFObject* object, Color color, Vec3 p, double maxDistance, const std::vector<std::unique_ptr<LightSource>>& lights, const std::vector<bool>& directLighting)
{
	Vec3 lightColor{ 0.0 };
	double totalLight{ 0.0 };

	if (object) {
		for (size_t i{}; i < lights.size(); ++i) {
			LightSource* light = lights[i].get();

			Vec3 lightDir = (light->position() - samplePoint).getNormal();
			double lightDist = light->position().distance(samplePoint);

			double attenuation = 1.0 / (lightDist * lightDist);
			double directLight = directLighting[i] ? light->luminance() * attenuation : 0.0;
			double ambLight = light->luminance() * light->ambientPower() * attenuation;

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

	updateRenderChar(totalLight);
	renderColor = camDistance >= maxDistance ? 0 : lightColor * 255;

	updateColorCode();
}

void PixelSample::updateColorCode()
{
	char* ptr = colorCodeData;

	std::memcpy(ptr, "\033[38;2;", 7);
	ptr += 7;

	std::to_chars_result res = std::to_chars(ptr, ptr + 4, static_cast<int>(renderColor.x));
	ptr = res.ptr;
	*ptr++ = ';';

	res = std::to_chars(ptr, ptr + 4, static_cast<int>(renderColor.y));
	ptr = res.ptr;
	*ptr++ = ';';

	res = std::to_chars(ptr, ptr + 4, static_cast<int>(renderColor.z));
	ptr = res.ptr;
	*ptr++ = 'm';

	colorCodeLen = static_cast<uint8_t>(ptr - colorCodeData);
}

std::string_view PixelSample::getColorCode() const
{
	return std::string_view(colorCodeData, colorCodeLen);
}

void PixelSample::updateRenderChar(double lightVal)
{
	size_t index = static_cast<size_t>(lightVal * (s_charGradient.length() - 1));
	renderChar = s_charGradient[index];
}
