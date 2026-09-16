#pragma once
#include "Vec3.h"
#include <string>
#include <vector>
#include "SDFObject.h"
#include "LightSource.h"

class PixelSample
{
private:
	static const std::string charGradient;
public:
	double camDistance{};
	double surfDistance{};
	Vec3 samplePoint{};
	size_t objectIndex{};
	Vec3 normal{};
	Color renderColor{};
	char renderChar{};
	std::string colorCode{};

	PixelSample() = default;
	PixelSample(double camDist, double surfDist, Vec3 point) : camDistance(camDist), surfDistance(surfDist), samplePoint(point) {}
	void buildRenderParams(const SDFObject* object, Color color, Vec3 p, double maxDistance, const std::vector<LightSource*>& lights);
};

