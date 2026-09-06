#include <cmath>
#include <limits>
#include "Vec2.h"

const Vec2 Vec2::Up{ 0.0, 1.0 };
const Vec2 Vec2::Down{ 0.0, -1.0 };
const Vec2 Vec2::Left{ -1.0, 0.0 };
const Vec2 Vec2::Right{ 1.0, 0.0 };
const Vec2 Vec2::One{ 1.0 };
const Vec2 Vec2::Zero{ 0.0 };

double Vec2::length() const
{
    return std::hypot(x, y);
}

double Vec2::distance(const Vec2& p) const
{
    return (p - *this).length();
}

double Vec2::dot(const Vec2& p)
{
    return x * p.x + y * p.y;
}

Vec2 Vec2::maximum(double m)
{
    return Vec2(m > x ? m : x, m > y ? m : y);
}

Vec2 Vec2::minimum(double m)
{
    return Vec2(m < x ? m : x, m < y ? m : y);
}

Vec2 Vec2::cross(const Vec2& p)
{
    return Vec2(x * p.y - y * p.x);
}

void Vec2::normalize()
{
    const double m = length();
    if (!m) return;
    x /= m; y /= m;
}

Vec2 Vec2::rotate(const double rads) const
{
    double c = cos(rads);
    double s = sin(rads);
    return Vec2(x * c - y * s, x * s + y * c);
}

Vec2 Vec2::getNormal() const
{
    const double m = length();
    return !m ? Vec2{} : Vec2(x / m, y / m);
}

Vec2 Vec2::operator+(const Vec2& p) const
{
    return Vec2{ x + p.x, y + p.y };
}

Vec2 Vec2::operator-(const Vec2& p) const
{
    return Vec2{ x - p.x, y - p.y};
}

Vec2 Vec2::operator*(const Vec2& p) const
{
    return Vec2{ x * p.x, y * p.y};
}

Vec2 Vec2::operator/(const Vec2& p) const noexcept
{
    return Vec2{ p.x == 0 ? 0.0 : x / p.x, p.y == 0 ? 0.0 : y / p.y};
}

Vec2& Vec2::operator+=(const Vec2& p)
{
    x += p.x; y += p.y;
    return *this;
}

Vec2& Vec2::operator-=(const Vec2& p)
{
    x -= p.x; y -= p.y;
    return *this;
}

Vec2& Vec2::operator*=(const Vec2& p)
{
    x *= p.x; y *= p.y;
    return *this;
}

Vec2& Vec2::operator/=(const Vec2& p)
{
    x = p.x == 0.0 ? 0.0 : x / p.x; y = p.y == 0.0 ? 0.0 : p.y;
    return *this;
}

Vec2 Vec2::operator*(const double d) const {
    return Vec2(x * d, y * d);
}

bool Vec2::operator==(const Vec2& p) const {
    return (x == p.x && y == p.y);
}

std::ostream& operator<<(std::ostream& os, const Vec2& p)
{
    return os << "Point3D[" << p.x << ", " << p.y <<"]";
}

Vec2 abs(const Vec2 v)
{
    return Vec2(abs(v.x), abs(v.y));
}
