#pragma once
#include <vector>
#include <ranges>
#include "PixelField.h"
#include "Vec3.h"
#include "SDFObject.h"
#include "Camera.h"
#include "SDFScene.h"

class RayMarcher
{
private:
	const double s_normalCheckOffset{ 0.001 };
	const double s_shadowTolerance{ 0.01 };
	const int m_maxSamples{ 300 };
	double m_tolerance;
	double m_maxDistance;

	void marchRay(Vec3 ray, Vec3 camPos, Color* color, const SDFObject* objects, PixelSample& sample, double maxDist, Vec3* lightPos = nullptr);
	void SampleScene(Vec3 p, Color* color, const SDFObject* objects, double & surfDist, size_t& objectIndex, double maxDist, Vec3* lightPos = nullptr);

public:
	RayMarcher(const double maxDistance, const double tolerance = 0.001)
		: m_maxDistance(maxDistance)
		, m_tolerance(tolerance) {}
	void march(SDFScene& scene, PixelField& field, const Camera& camera);
};

