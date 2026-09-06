#pragma once
#include "Vec3.h"
#include <string>
#include <vector>
#include "SceneObject.h"
#include "LightSource.h"

class SceneSample
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
	std::string colorCode;

	SceneSample() = default;
	SceneSample(double camDist, double surfDist, Vec3 point) : camDistance(camDist), surfDistance(surfDist), samplePoint(point) {}
	void buildRenderParams(const std::unique_ptr<SceneObject>& object, Vec3 p, double maxDistance, bool lit, const std::vector<LightSource*>& lights, double time);
};

