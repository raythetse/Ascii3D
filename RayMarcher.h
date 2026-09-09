#pragma once
#include <vector>
#include <ranges>
#include "PixelField.h"
#include "Vec3.h"
#include "SDFObject.h"
#include "Camera.h"

class RayMarcher
{
private:
	const double s_normalCheckOffset{ 0.001 };
	const double s_shadowTolerance{ 0.1 };
	const int m_maxSamples{ 300 };
	double m_stepFactor;
	double m_tolerance;
	double m_maxDistance;

	void marchRay(Vec3 ray, Vec3 p, const std::vector<std::unique_ptr<SDFObject>>& objects, PixelSample& sample, double* shadowMax = nullptr, Vec3* lightPos = nullptr);
	void SampleScene(Vec3 p, const std::vector<std::unique_ptr<SDFObject>>& objects, double & surfDist, size_t& objectIndex, double* shadowMax = nullptr, Vec3* lightPos = nullptr);

public:
	RayMarcher(const double maxDistance, const double stepFactor = 1.0, const double tolerance = 0.001)
		: m_maxDistance(maxDistance)
		, m_stepFactor(stepFactor)
		, m_tolerance(tolerance) {}
	void march(	PixelField& field,
				const std::vector<std::unique_ptr<SDFObject>>& objects,
				const std::vector<std::unique_ptr<LightSource>>& lights,
				const Camera& camera,
				std::ranges::iota_view<size_t, size_t>& range);
};

