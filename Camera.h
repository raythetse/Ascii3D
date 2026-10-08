#pragma once
#include "Vec3.h"
#include "Vec2.h"
#include <vector>
#include <windows.h>

class Camera
{
private:
	Vec3 m_position{ 0.0, 1.8, -5.0 };
	Vec3 m_initialPosition{ 0.0, 1.8, -5.0 };
	Vec3 m_rotation{};
	Vec3 m_forward{ 0.0, 0.0, 1.0 };
	Vec3 m_up{ 0.0, 1.0, 0.0 };
	Vec3 m_right{ 1.0, 0.0, 0.0 };
	Vec2 m_viewportSize{};
	double m_lookSpeed{ .0035 };
	double m_moveSpeed{ 3.0 };

public:
	Camera() = default;
	void update();
	void reset();
	void changeMoveSpeed(double delta);
	Vec3 getViewportRay(const double x, const double y) const;
	Vec2 setViewportSize(Vec2 size);
	Vec3 getPosition() const { return m_position; }
	Vec3 getForward() const { return m_forward; }
	Vec3 getRight() const { return m_right; }
	Vec3 getUp() const { return m_up; }
};

