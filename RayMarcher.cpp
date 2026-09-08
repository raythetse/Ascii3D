#include "RayMarcher.h"
#include <algorithm>
#include <cmath>
#include <execution>
#include <ranges>

void RayMarcher::marchRay(Vec3 ray, Vec3 camPos, const std::vector<std::unique_ptr<SceneObject>>& objects, SceneSample& sample)
{
	double rayDist{ 0 };
    double surfDist{ m_maxDistance };
    int samples{};
    size_t objectIndex{};

	while (rayDist < m_maxDistance && samples <= m_maxSamples) {
        Vec3 p = camPos + ray * rayDist;
        SampleScene(p, objects, surfDist, objectIndex);
        if (surfDist < m_tolerance) {
            sample.camDistance = rayDist;
            sample.samplePoint = p;
            sample.surfDistance = surfDist;
            sample.objectIndex = objectIndex;
            return;
        }
		rayDist += m_stepFactor * surfDist;
        ++samples;
	}
    sample.camDistance = m_maxDistance;
}

void RayMarcher::SampleScene(Vec3 p, const std::vector<std::unique_ptr<SceneObject>>& objects, double& surfDist, size_t& objectIndex)
{
    Vec3 color{};
    surfDist = m_maxDistance;

    for (int i{}; i < objects.size(); ++i) {
        double d = objects[i]->signedDistance(p);
        if (d < surfDist) {
            surfDist = d;
            if (surfDist < m_tolerance) {
                objectIndex = i;
                return;
            }
        }
    }
}

void RayMarcher::march( PixelField& field,
                        const std::vector<std::unique_ptr<SceneObject>>& objects,
                        const std::vector<std::unique_ptr<LightSource>>& lights,
                        const Camera& camera,
                        std::ranges::iota_view<size_t, size_t>& range)
{
    const size_t width = field.width();
    const size_t height = field.height();

    // parallel execution policy parallelizes the iterations automatically
    std::for_each(std::execution::par, range.begin(), range.end(),
        [this, &field, &objects, &camera, &lights, width, height](size_t index)
    {
        size_t row = index / width;
        size_t col = index % width;

        Vec3 ray = camera.getViewportRay(static_cast<double>(col) / static_cast<double>(width), static_cast<double>(row) / static_cast<double>(height)).getNormal();
        marchRay(ray, camera.getPosition(), objects, field(row, col));

        SceneSample& sample = field(row, col);
        std::vector<LightSource*> directLights{};
        // Get normal
        bool hit = sample.camDistance < m_maxDistance;
        if (hit) {
            auto& hitObj = objects[sample.objectIndex];

            Vec3 p = sample.samplePoint - ray * m_tolerance;
            Vec3 offsetX = Vec3(s_normalCheckOffset, 0.0, 0.0);
            Vec3 offsetY = Vec3(0.0, s_normalCheckOffset, 0.0);
            Vec3 offsetZ = Vec3(0.0, 0.0, s_normalCheckOffset);

            double dx = hitObj->signedDistance(p + offsetX) - hitObj->signedDistance(p - offsetX);
            double dy = hitObj->signedDistance(p + offsetY) - hitObj->signedDistance(p - offsetY);
            double dz = hitObj->signedDistance(p + offsetZ) - hitObj->signedDistance(p - offsetZ);

            sample.normal = Vec3(dx, dy, dz).getNormal();

            for (auto& light : lights) {
                SceneSample shadowSample;
                Vec3 diff = sample.samplePoint - light->position();
                marchRay(diff.getNormal(), light->position(), objects, shadowSample);
                if (shadowSample.objectIndex == sample.objectIndex) {
                //if ((shadowSample.samplePoint - sample.samplePoint).length() < m_tolerance) { // Attempt at self-shadowing repeats, too many errors
                    auto l = light.operator->();
                    directLights.push_back(light.operator->());
                }
            }

            sample.buildRenderParams(hitObj, sample.samplePoint, m_maxDistance, objects[sample.objectIndex]->lit(), directLights);
        }
        else {
            sample.buildRenderParams(nullptr, sample.samplePoint, m_maxDistance, true, directLights);
        }
    });
}


