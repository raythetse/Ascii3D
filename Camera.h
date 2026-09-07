#pragma once
#include "Vec3.h"
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
	double m_charRatio{ 2.0 };
	double m_viewportWidth;
	double m_viewportHeight;
	double m_lookSpeed;
	double m_moveSpeed;
	HWND m_console;
	HCURSOR m_oldCursor{};
	POINT m_screenCentre;

public:
	Camera(const double viewportWidth, const double viewportHeight, const Vec3 position = { 0.0, 0.0, -5.0 }, const double lookSpeed = 3.5, const double moveSpeed = 3.0);
	void update(const double deltaTime);
	void reset();
	Vec3 getPosition() const { return m_position; }
	Vec3 getViewportRay(const double x, const double y) const;
};

