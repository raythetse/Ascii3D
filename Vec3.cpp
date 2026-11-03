#include <cmath>
#include <algorithm>
#include <format>
#include "MathFunctions.h"
#include "Vec3.h"

const Vec3 Vec3::Up{ 0.0, 1.0, 0.0 };
const Vec3 Vec3::Down{ 0.0, -1.0, 0.0 };
const Vec3 Vec3::Left{ -1.0, 0.0, 0.0 };
const Vec3 Vec3::Right{ 1.0, 0.0, 0.0 };
const Vec3 Vec3::Forward{ 0.0, 0.0, 1.0 };
const Vec3 Vec3::Backward{ 0.0, 0.0, -1.0 };
const Vec3 Vec3::One{ 1.0 };
const Vec3 Vec3::Zero{ 0.0 };

const Vec3 Vec3::Black{ 0.0, 0.0, 0.0 };
const Vec3 Vec3::LightGray{ 0.75, 0.75, 0.75 };
const Vec3 Vec3::MediumGray{ 0.5, 0.5, 0.5 };
const Vec3 Vec3::DarkGray{ 0.25, 0.25, 0.25 };
const Vec3 Vec3::White{ 1.0, 1.0, 1.0 };

const Vec3 Vec3::Red{ 1.0, 0.0, 0.0 };
const Vec3 Vec3::Orange{ 1.0, 0.5, 0.0 };
const Vec3 Vec3::DeepPink{ 1.0, 0.0, 0.5 };
const Vec3 Vec3::Pink{ 1.0, 0.5, 0.5 };

const Vec3 Vec3::Green{ 0.0, 1.0, 0.0 };
const Vec3 Vec3::ChartreuseGreen{ 0.5, 1.0, 0.0 };
const Vec3 Vec3::SpringGreen{ 0.0, 1.0, 0.5 };
const Vec3 Vec3::LightGreen{ 0.5, 1.0, 0.5 };

const Vec3 Vec3::Blue{ 0.0, 0.0, 1.0 };
const Vec3 Vec3::Violet{ 0.5, 0.0, 1.0 };
const Vec3 Vec3::Indigo{ 0.3, 0.0, 0.51 };
const Vec3 Vec3::AzureBlue{ 0.0, 0.5, 1.0 };
const Vec3 Vec3::LightBlue{ 0.5, 0.5, 1.0 };
const Vec3 Vec3::SkyBlue{ 0.53, 0.80, 0.92 };

const Vec3 Vec3::Yellow{ 1.0, 1.0, 0.0 };
const Vec3 Vec3::Magenta{ 1.0, 0.0, 1.0 };
const Vec3 Vec3::Teal{ 0.0, 1.0, 1.0 };

const Vec3 Vec3::Brown{ 0.4, 0.33, 0.18 };
const Vec3 Vec3::Gold{ 1.0, 0.87, 0.0 };

double Vec3::length() const
{
    return std::hypot(x, y, z);
}

double Vec3::distance(const Vec3& p) const
{
    return (p - *this).length();
}

double Vec3::dot(const Vec3& p) const
{
    return x * p.x + y * p.y + z * p.z;
}

Vec3 Vec3::maximum(double m) const
{
    return { m > x ? m : x, m > y ? m : y, m > z ? m : z };
}

Vec3 Vec3::minimum(double m) const
{
    return { m < x ? m : x, m < y ? m : y, m < z ? m : z };
}

Vec3 Vec3::clamp(double min, double max) const
{
    return maximum(min).minimum(max);
}

Vec3 Vec3::clamp(Vec3 min, Vec3 max) const
{
    return Vec3{ std::clamp(x, min.x, max.x), std::clamp(y, min.y, max.y), std::clamp(z, min.z, max.z) };
}

Vec3 Vec3::saturate() const
{
    return Vec3::clamp(0.0, 1.0);
}

Vec3 Vec3::cross(const Vec3& p) const
{
    return { y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x };
}

Vec3 Vec3::lerp(Vec3 v, double weight) const
{
    return { std::lerp(x, v.x, weight), std::lerp(y, v.y, weight), std::lerp(z, v.z, weight) };
}

Vec3 Vec3::sin() const
{
    return { std::sin(x), std::sin(y), std::sin(z) };
}

Vec3 Vec3::cos() const
{
    return { std::cos(x), std::cos(y), std::cos(z) };
}

std::pair<Vec3, Vec3> Vec3::sincos() const
{
    Vec2 scX = MF::fast_sincos(x);
    Vec2 scY = MF::fast_sincos(y);
    Vec2 scZ = MF::fast_sincos(z);
    return { { scX.x, scY.x, scZ.x }, { scX.y, scY.y, scZ.y } };
}

void Vec3::normalize()
{
    const double m = length();
    if (!m) return;
    x /= m; y /= m; z /= m;
}

Vec3 Vec3::rotateXAxis(const double rads) const
{
    double c = std::cos(rads);
    double s = std::sin(rads);
    return { x, y * c - z * s, y * s + z * c };
}

Vec3 Vec3::rotateYAxis(const double rads) const
{
    double c = std::cos(rads);
    double s = std::sin(rads);
    return { x * c + z * s, y, -x * s + z * c };
}

Vec3 Vec3::rotateZAxis(const double rads) const
{
    double c = std::cos(rads);
    double s = std::sin(rads);
    return { x * c - y * s, x * s + y * c, z };
}

Vec3 Vec3::getNormal() const
{
    const double m = length();
    return !m ? Vec3{} : Vec3{ x / m, y / m, z / m };
}

Vec3 Vec3::quantize(size_t level) const
{
    if (level <= 0) return *this;
    return { std::round(level * x) / level, std::round(level * y) / level, std::round(level * z) / level };
}

Vec3 Vec3::floor() const
{
    return { std::floor(x), std::floor(y), std::floor(z) };
}

Vec3 Vec3::ceil() const
{
    return { std::ceil(x), std::ceil(y), std::ceil(z) };
}

Vec3 Vec3::round() const
{
    return { std::round(x), std::round(y), std::round(z) };
}

Vec3 Vec3::abs() const
{
    return { std::abs(x), std::abs(y), std::abs(z) };
}

Vec3 Vec3::fmod(Vec3 v) const
{
    return { std::fmod(x, v.x), std::fmod(y, v.y), std::fmod(z, v.z) };
}

Vec3 Vec3::fmod(double d) const
{
    return { std::fmod(x, d), std::fmod(y, d), std::fmod(z, d) };
}

Vec3 Vec3::operator-() const
{
    return { -x, -y, -z };
}

Vec3 Vec3::operator+(const Vec3& p) const
{
    return { x + p.x, y + p.y, z + p.z };
}

Vec3 Vec3::operator-(const Vec3& p) const
{
    return { x - p.x, y - p.y, z - p.z };
}

Vec3 Vec3::operator*(const Vec3& p) const
{
    return { x * p.x, y * p.y, z * p.z };
}

Vec3 Vec3::operator/(const Vec3& p) const noexcept
{
    return { p.x == 0.0 ? 0.0 : x / p.x, p.y == 0.0 ? 0.0 : y / p.y, p.z == 0.0 ? 0.0 : z / p.z };
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

Vec3& Vec3::operator/=(const Vec3& p) noexcept
{
    x = p.x == 0.0 ? 0.0 : x / p.x;
    y = p.y == 0.0 ? 0.0 : y / p.y;
    z = p.z == 0.0 ? 0.0 : z / p.z;
    return *this;
}

Vec3 Vec3::operator+(const double d) const
{
    return { x + d, y + d, z + d };
}

Vec3 Vec3::operator-(const double d) const
{
    return { x - d, y - d, z - d };
}

Vec3 Vec3::operator*(const double d) const
{
    return { x * d, y * d, z * d };
}

Vec3 Vec3::operator/(const double d) const
{
    return d == 0.0 ? Vec3::Zero : *this / Vec3{ d };
}

Vec3& Vec3::operator=(const double d)
{
    return *this = Vec3{ d };
}

Vec3& Vec3::operator+=(const double d)
{
    return *this += Vec3{ d };
}

Vec3& Vec3::operator-=(const double d)
{
    return *this -= Vec3{ d };
}

Vec3& Vec3::operator*=(const double d)
{
    return *this *= Vec3{ d };
}

Vec3& Vec3::operator/=(const double d)
{
    return *this /= Vec3{ d };
}

bool Vec3::operator==(const double d) const {
    return (x == d && y == d && z == d);
}

Vec3::operator bool() const
{
    return *this != Vec3::Zero;
}

std::string Vec3::getShader() const
{
    return std::format("vec3({:.7f}, {:.7f}, {:.7f})", static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
}

std::ostream& operator<<(std::ostream& os, const Vec3& p)
{
    return os << p.x << ' ' << p.y << ' ' << p.z;
}

std::istream& operator>>(std::istream& is, Vec3& p)
{
    return is >> p.x >> p.y >> p.z;
}
