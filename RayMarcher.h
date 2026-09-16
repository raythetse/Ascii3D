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
	const double s_shadowTolerance{ 0.1 };
	const int m_maxSamples{ 300 };
	double m_stepFactor;
	double m_tolerance;
	double m_maxDistance;

	void marchRay(Vec3 ray, Vec3 camPos, Color* color, const SDFObject* objects, PixelSample& sample, double* shadowMax = nullptr, Vec3* lightPos = nullptr);
	void SampleScene(Vec3 p, Color* color, const SDFObject* objects, double & surfDist, size_t& objectIndex, double* shadowMax = nullptr, Vec3* lightPos = nullptr);

public:
	RayMarcher(const double maxDistance, const double stepFactor = 1.0, const double tolerance = 0.001)
		: m_maxDistance(maxDistance)
		, m_stepFactor(stepFactor)
		, m_tolerance(tolerance) {}
	void march(SDFScene& scene, PixelField& field, const Camera& camera);
};

