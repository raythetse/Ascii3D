#include <cmath>
#include "Vec3.h"

const Vec3 Vec3::Up{ 0.0, 1.0, 0.0 };
const Vec3 Vec3::Down{ 0.0, -1.0, 0.0 };
const Vec3 Vec3::Left{ -1.0, 0.0, 0.0 };
const Vec3 Vec3::Right{ 1.0, 0.0, 0.0 };
const Vec3 Vec3::Forward{ 0.0, 0.0, 1.0 };
const Vec3 Vec3::Backward{ 0.0, 0.0, -1.0 };
const Vec3 Vec3::One{ 1.0 };
const Vec3 Vec3::Zero{ 0.0 };

double Vec3::length() const
{
    return std::hypot(x, y, z);
}

double Vec3::distance(const Vec3& p) const
{
    return (p - *this).length();
}

double Vec3::dot(const Vec3& p)
{
    return x * p.x + y * p.y + z * p.z;
}

Vec3 Vec3::maximum(double m)
{
    return Vec3(m > x ? m : x, m > y ? m : y, m > z ? m : z);
}

Vec3 Vec3::minimum(double m)
{
    return Vec3(m < x ? m : x, m < y ? m : y, m < z ? m : z);
}

Vec3 Vec3::cross(const Vec3& p)
{
    return Vec3(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x);
}

Vec3 Vec3::lerp(Vec3 v, double weight) const
{
    return { std::lerp(x, v.x, weight), std::lerp(y, v.y, weight), std::lerp(z, v.z, weight) };
}

void Vec3::normalize()
{
    const double m = length();
    if (!m) return;
    x /= m; y /= m; z /= m;
}

Vec3 Vec3::rotateXAxis(const double rads) const
{
    double c = cos(rads);
    double s = sin(rads);
    return Vec3(x, y * c - z * s, y * s + z * c);
}

Vec3 Vec3::rotateYAxis(const double rads) const
{
    double c = cos(rads);
    double s = sin(rads);
    return Vec3(x * c + z * s, y, -x * s + z * c);
}

Vec3 Vec3::rotateZAxis(const double rads) const
{
    double c = cos(rads);
    double s = sin(rads);
    return Vec3(x * c - y * s, x * s + y * c, z);
}

Vec3 Vec3::getNormal() const
{
    const double m = length();
    return !m ? Vec3{} : Vec3(x / m, y / m, z / m);
}

Vec3 Vec3::quantize(size_t level)
{
    if (level <= 0) return *this;
    return {std::floor(level * x) / level, std::floor(level * y) / level, std::floor(level * z) / level};
}

Vec3 Vec3::floor() const
{
    return Vec3(x - std::floor(x), y - std::floor(y), z - std::floor(z));
}

Vec3 Vec3::operator+(const Vec3& p) const
{
    return Vec3{ x + p.x, y + p.y, z + p.z };
}

Vec3 Vec3::operator-(const Vec3& p) const
{
    return Vec3{ x - p.x, y - p.y, z - p.z };
}

Vec3 Vec3::operator*(const Vec3& p) const
{
    return Vec3{ x * p.x, y * p.y, z * p.z };
}

Vec3 Vec3::operator/(const Vec3& p) const
{
    return Vec3{ p.x == 0.0 ? 0.0 : x / p.x, p.y == 0.0 ? 0.0 : y / p.y, p.z == 0.0 ? 0.0 : z / p.z };
}
Vec3& Vec3::operator=(const Vec3& p) {
    x = p.x; y = p.y; z = p.z;
    return *this;
}

Vec3& Vec3::operator+=(const Vec3& p)
{
    x += p.x; y += p.y; z += p.z;
    return *this;
}

Vec3& Vec3::operator-=(const Vec3& p)
{
    x -= p.x; y -= p.y; z -= p.z;
    return *this;
}

Vec3& Vec3::operator*=(const Vec3& p)
{
    x *= p.x; y *= p.y; z *= p.z;
    return *this;
}

Vec3& Vec3::operator/=(const Vec3& p)
{
    x = p.x == 0 ? 0 : p.x; y = p.y == 0.0 ? 0.0 : y / p.y; z = p.z == 0.0 ? 0.0 : z / p.z;
    return *this;
}

Vec3 Vec3::operator*(const double d) const {
    return Vec3(x * d, y * d, z * d);
}

bool Vec3::operator==(const Vec3& p) const {
    return (x == p.x && y == p.y && z == p.z);
}

Vec3::operator bool() const
{
    return *this != Vec3::Zero;
}

std::ostream& operator<<(std::ostream& os, const Vec3& p)
{
    return os << "Point3D[" << p.x << ", " << p.y << ", " << p.z << "]";
}

Vec3 abs(const Vec3 v)
{
    return Vec3(abs(v.x), abs(v.y), abs(v.z));
}
