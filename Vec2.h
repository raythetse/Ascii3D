#pragma once
#include <ostream>

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
	Vec2 maximum(const double m);
	Vec2 minimum(const double m);
	Vec2 maximum(const Vec2 m);
	Vec2 minimum(const Vec2 m);
	Vec2 cross(const Vec2& p);
	Vec2 abs();
	void normalize();
	Vec2 rotate(const double rads) const ;
	Vec2 getNormal() const ;
	Vec2 operator-(const Vec2& p) const;
	Vec2 operator*(const Vec2& p) const;
	Vec2 operator/(const Vec2& p) const noexcept;
	Vec2 operator+(const Vec2& p) const;
	Vec2& operator+=(const Vec2& p);
	Vec2& operator-=(const Vec2& p);
	Vec2& operator*=(const Vec2& p);
	Vec2& operator/=(const Vec2& p);
	Vec2 operator*(const double d) const;
	bool operator==(const Vec2& p) const;
	std::string getShaderVec2() const;
};

std::ostream& operator<<(std::ostream& os, const Vec2& p);
Vec2 abs(const Vec2 v);