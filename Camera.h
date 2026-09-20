#pragma once
#include "Vec3.h"
#include "Vec2.h"
#include <windows.h>

class Camera
{
private:
	Vec3 m_position{};
	Vec3 m_initialPosition{};
	Vec3 m_rotation{};
	Vec3 m_forward{ 0.0, 0.0, 1.0 };
	Vec3 m_up{ 0.0, 1.0, 0.0 };
	Vec3 m_right{ 1.0, 0.0, 0.0 };
	Vec2 m_viewportSize{};
	double m_lookSpeed;
	double m_moveSpeed;

public:
	Camera(const double viewportWidth, const double viewportHeight, const Vec3 position = { 0.0, 0.0, -5.0 }, const double lookSpeed = 3.5, const double moveSpeed = 3.0)
		: m_viewportSize(viewportWidth, viewportHeight), m_position(position)
		, m_initialPosition(position), m_lookSpeed(lookSpeed * 0.001), m_moveSpeed(moveSpeed) {}
	void update();
	void reset();
	Vec3 getViewportRay(const double x, const double y) const;
	void setViewportSize(Vec2 size);
	Vec3 getPosition() const { return m_position; }
	Vec3 getForward() const { return m_forward; }
	Vec3 getRight() const { return m_right; }
	Vec3 getUp() const { return m_up; }
};

