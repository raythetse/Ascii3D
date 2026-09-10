#include "DistortOperation.h"
#include "Mat2.h"

void Twist::transform(Vec3& p) const
{
	Vec2 sc = MF::fast_sincos(m_magnitude * p.y);
	Mat2  m = Mat2{ { sc.y, -sc.x, sc.x, sc.y} };
	Vec2 xz = m * Vec2{ p.x, p.z };
	p = Vec3(xz.x, xz.y, p.y);
}

void Bend::transform(Vec3& p) const
{
	Vec2 sc = MF::fast_sincos(m_magnitude * p.x);
	Mat2  m = Mat2{ { sc.y, -sc.x, sc.x, sc.y } };
	Vec2 xy = m * Vec2{ p.x, p.y };
	p = Vec3(xy.x, xy.y, p.z);
}

void* DistortOperation::getPropertyPtr(std::string_view propName)
{
	if (propName == "magnitude") return &m_magnitude;
	return nullptr;
}