#pragma once
#include "Vec2.h"
#include <cmath>
#include <format>
#include <limits>
#include "MathFunctions.h"

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

double Vec2::dot(const Vec2& p) const
{
    return x * p.x + y * p.y;
}

Vec2 Vec2::maximum(const double m)
{
    return { m > x ? m : x, m > y ? m : y };
}

Vec2 Vec2::minimum(const double m)
{
    return { m < x ? m : x, m < y ? m : y };
}

Vec2 Vec2::maximum(const Vec2 m)
{
    return Vec2{ x > m.x ? x : m.x, y > m.y ? y : m.y };
}

Vec2 Vec2::minimum(const Vec2 m)
{
    return Vec2{ x < m.x ? x : m.x, y < m.y ? y : m.y };
}

Vec2 Vec2::cross(const Vec2& p)
{
    return (x * p.y - y * p.x);
}

Vec2 Vec2::abs()
{
    return { std::abs(x), std::abs(y) };
}

void Vec2::normalize()
{
    const double m = length();
    if (!m) return;
    x /= m; y /= m;
}

Vec2 Vec2::rotate(const double rads) const
{
    Vec2 sc = MF::fast_sincos(rads);
    return { x * sc.y - y * sc.x, x * sc.x + y * sc.y };
}

Vec2 Vec2::getNormal() const
{
    const double m = length();
    return !m ? Vec2{} : Vec2(x / m, y / m);
}

Vec2 Vec2::operator+(const Vec2& p) const
{
    return { x + p.x, y + p.y };
}

Vec2 Vec2::operator-(const Vec2& p) const
{
    return { x - p.x, y - p.y};
}

Vec2 Vec2::operator*(const Vec2& p) const
{
    return { x * p.x, y * p.y};
}

Vec2 Vec2::operator/(const Vec2& p) const noexcept
{
    return { p.x == 0 ? 0.0 : x / p.x, p.y == 0 ? 0.0 : y / p.y};
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
    return { x * d, y * d };
}

bool Vec2::operator==(const Vec2& p) const {
    return (x == p.x && y == p.y);
}

std::string Vec2::getShader() const
{
    return std::format("vec2({:.7f}, {:.7f})", static_cast<float>(x), static_cast<float>(y));
}

std::ostream& operator<<(std::ostream& os, const Vec2& p)
{
    return os << "Point3D[" << p.x << ", " << p.y <<"]";
}

Vec2 abs(const Vec2 v)
{
    return { abs(v.x), abs(v.y) };
}
