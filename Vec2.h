#pragma once
#include <istream>
#include <ostream>
#include <string>

class Vec2
{
public:
	static const Vec2 Up;
	static const Vec2 Down;
	static const Vec2 Right;
	static const Vec2 Left;
	static const Vec2 One;
	static const Vec2 Zero;

	double x{};
	double y{};

	Vec2() = default;
	Vec2(const double x, const double y) : x(x), y(y) {}
	Vec2(const double v) : x(v), y(v) {}
	double length() const;
	double distance(const Vec2& p) const;
	double dot(const Vec2& p) const;
	Vec2 maximum(const double m) const;
	Vec2 minimum(const double m) const;
	Vec2 maximum(const Vec2 m) const;
	Vec2 minimum(const Vec2 m) const;
	// The z component of the 3D cross product (a scalar; the signed parallelogram area)
	double cross(const Vec2& p) const;
	Vec2 sin() const;
	Vec2 cos() const;
	Vec2 abs() const;
	Vec2 fmod(Vec2 v) const;
	Vec2 fmod(double d) const;
	void normalize();
	Vec2 rotate(const double rads) const;
	Vec2 getNormal() const;
	Vec2 operator-() const;
	Vec2 operator+(const Vec2& p) const;
	Vec2 operator-(const Vec2& p) const;
	Vec2 operator*(const Vec2& p) const;
	Vec2 operator/(const Vec2& p) const noexcept;
	Vec2 operator+(const double d) const;
	Vec2 operator-(const double d) const;
	Vec2 operator*(const double d) const;
	Vec2 operator/(const double d) const noexcept;
	Vec2& operator+=(const Vec2& p);
	Vec2& operator-=(const Vec2& p);
	Vec2& operator*=(const Vec2& p);
	Vec2& operator/=(const Vec2& p);
	Vec2& operator+=(const double d);
	Vec2& operator-=(const double d);
	Vec2& operator*=(const double d);
	Vec2& operator/=(const double d);
	bool operator==(const Vec2& p) const = default;
	bool operator==(const double d) const;
	std::string getShader() const;
};

std::ostream& operator<<(std::ostream& os, const Vec2& p);
std::istream& operator>>(std::istream& os, Vec2& p);