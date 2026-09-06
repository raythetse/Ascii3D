#pragma once
#include <ostream>

class Vec3
{
public:
	static const Vec3 Up;
	static const Vec3 Down;
	static const Vec3 Right;
	static const Vec3 Left;
	static const Vec3 Forward;
	static const Vec3 Backward;
	static const Vec3 One;
	static const Vec3 Zero;

	double x{};
	double y{};
	double z{};

	Vec3() = default;
	Vec3(const double x, const double y, const double z) : x(x), y(y), z(z) {}
	Vec3(const double v) : x(v), y(v), z(v) {}
	double length() const;
	double distance(const Vec3& p) const;
	double dot(const Vec3& p);
	Vec3 maximum(double m);
	Vec3 minimum(double m);
	Vec3 cross(const Vec3& p);
	Vec3 lerp(Vec3 v, double weight) const;
	void normalize();
	Vec3 rotateXAxis(const double rads) const ;
	Vec3 rotateYAxis(const double rads) const ;
	Vec3 rotateZAxis(const double rads) const ;
	Vec3 getNormal() const ;
	Vec3 quantize(size_t level);
	Vec3 floor() const;
	Vec3 operator-(const Vec3& p) const;
	Vec3 operator*(const Vec3& p) const;
	Vec3 operator/(const Vec3& p) const;
	Vec3 operator+(const Vec3& p) const;
	Vec3& operator=(const Vec3& p);
	Vec3& operator+=(const Vec3& p);
	Vec3& operator-=(const Vec3& p);
	Vec3& operator*=(const Vec3& p);
	Vec3& operator/=(const Vec3& p);
	Vec3 operator*(const double d) const;
	bool operator==(const Vec3& p) const;
	explicit operator bool() const;
};

std::ostream& operator<<(std::ostream& os, const Vec3& p);
Vec3 abs(const Vec3 v);

namespace Color {
	const Vec3 Black{ 0.0, 0.0, 0.0 };
	const Vec3 LightGray{ 0.75, 0.75, 0.75 };
	const Vec3 MediumGray{ 0.5, 0.5, 0.5 };
	const Vec3 DarkGray{ 0.25, 0.25, 0.25 };
	const Vec3 White{ 1.0, 1.0, 1.0 };

	const Vec3 Red{ 1.0, 0.0, 0.0 };
	const Vec3 Orange{ 1.0, 0.5, 0.0 };
	const Vec3 DeepPink{ 1.0, 0.0, 0.5 };
	const Vec3 Pink{ 1.0, 0.5, 0.5 };

	const Vec3 Green{ 0.0, 1.0, 0.0 };
	const Vec3 ChartreuseGreen{ 0.5, 1.0, 0.0 };
	const Vec3 SpringGreen{ 0.0, 1.0, 0.5 };
	const Vec3 LightGreen{ 0.5, 1.0, 0.5 };

	const Vec3 Blue{ 0.0, 0.0, 1.0 };
	const Vec3 Violet{ 0.5, 0.0, 1.0 };
	const Vec3 AzureBlue{ 0.0, 0.5, 1.0 };
	const Vec3 LightBlue{ 0.5, 0.5, 1.0 };

	const Vec3 Yellow{ 1.0, 1.0, 0.0 };
	const Vec3 Magenta{ 1.0, 0.0, 1.0 };
	const Vec3 Teal{ 0.0, 1.0, 1.0 };
}
