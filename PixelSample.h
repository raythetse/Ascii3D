#pragma once
#include "Vec3.h"
#include <string>
#include <vector>
#include "SDFObjectInc.h"
#include "LightSource.h"

struct PixelSample
{
private:
	static const std::string s_charGradient;

public:
	double camDistance{};
	double surfDistance{};
	Vec3 samplePoint{};
	size_t objectIndex{};
	Vec3 normal{};
	Color renderColor{};
	char renderChar{};
	char colorCodeData[24]{};
	uint8_t colorCodeLen{ 0 };

	PixelSample() = default;
	PixelSample(double camDist, double surfDist, Vec3 point) : camDistance(camDist), surfDistance(surfDist), samplePoint(point) {}
	void buildRenderParams(const SDFObject* object, Color color, Vec3 p, double maxDistance, const std::vector<std::unique_ptr<LightSource>>& lights, const std::vector<bool>& directLighting);
	void updateRenderChar(double lightVal);
	void updateColorCode();
	std::string_view getColorCode() const;
};

