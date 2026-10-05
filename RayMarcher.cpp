#include "RayMarcher.h"
#include <algorithm>
#include <cmath>
#include <execution>
#include <ranges>

void RayMarcher::marchRay(Vec3 ray, Vec3 camPos, Color* color, const SDFObject* object, PixelSample& sample, double maxDist, Vec3* lightPos)
{
	double rayDist{ 0 };
    double surfDist{ maxDist };
    int samples{};
    size_t objectIndex{};

	while (rayDist < (maxDist) && samples <= m_maxSamples) {
        Vec3 p = camPos + ray * rayDist;
        if (lightPos) surfDist = min(surfDist, (p - *lightPos).length());
        SampleScene(p, color, object, surfDist, objectIndex, maxDist);
        if (surfDist < m_tolerance) {
            sample.camDistance = rayDist;
            sample.samplePoint = p;
            sample.surfDistance = surfDist;
            sample.objectIndex = objectIndex;
            return;
        }
		rayDist += surfDist;
        ++samples;
	}
    sample.camDistance = maxDist;
}

void RayMarcher::SampleScene(Vec3 p, Color* color, const SDFObject* object, double& surfDist, size_t& objectIndex, double maxDist, Vec3* lightPos)
{
    surfDist = maxDist;

    double d = object->signedDistance(p, color);
    if (d < surfDist) {
        surfDist = d;
        if (surfDist < m_tolerance) return;
    }
}

void RayMarcher::march(SDFScene* scene, PixelField& field, const Camera& camera)
{
    auto* object = scene->baseObject();
    auto& lights = scene->lights();
    auto& range = field.range();
    // parallel execution policy parallelizes the iterations automatically
    std::for_each(std::execution::par, range.begin(), range.end(), [this, &field, object, &lights, &camera](size_t index)
    {
        size_t width = field.width();
        size_t height = field.height();
        size_t row = index / width;
        size_t col = index % width;

        Vec3 ray = camera.getViewportRay(static_cast<double>(col) / static_cast<double>(width), static_cast<double>(row) / static_cast<double>(height)).getNormal();
        Color color{ 0.0 };
        marchRay(ray, camera.getPosition(), &color, object, field(row, col), m_maxDistance);

        PixelSample& sample = field(row, col);
        std::vector<bool> directLighting{};

        bool hit = sample.camDistance < m_maxDistance;
        if (hit) {
            Vec3 p = sample.samplePoint - ray * m_tolerance;
            Vec3 offsetX = Vec3(s_normalCheckOffset, 0.0, 0.0);
            Vec3 offsetY = Vec3(0.0, s_normalCheckOffset, 0.0);
            Vec3 offsetZ = Vec3(0.0, 0.0, s_normalCheckOffset);

            double dx = object->signedDistance(p + offsetX) - object->signedDistance(p - offsetX);
            double dy = object->signedDistance(p + offsetY) - object->signedDistance(p - offsetY);
            double dz = object->signedDistance(p + offsetZ) - object->signedDistance(p - offsetZ);

            sample.normal = Vec3(dx, dy, dz).getNormal();

            for (auto& light : lights) {
                PixelSample shadowSample;
                Vec3 lightPos = light->position();
                Vec3 diff = lightPos - sample.samplePoint;
                double shadowMax = diff.length();
                Vec3 ray = diff.getNormal();
                marchRay(ray, sample.samplePoint + ray * s_shadowTolerance, nullptr, object, shadowSample, shadowMax, &lightPos);
                directLighting.push_back(shadowSample.camDistance == shadowMax);
            }
            sample.buildRenderParams(object, color, sample.samplePoint, m_maxDistance, lights, directLighting);
        }
        else {
            sample.buildRenderParams(nullptr, {}, sample.samplePoint, m_maxDistance, lights, directLighting);
        }
    });
}


