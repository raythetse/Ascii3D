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

	static const Vec3 Black;
	static const Vec3 LightGray;
	static const Vec3 MediumGray;
	static const Vec3 DarkGray;
	static const Vec3 White;
	
	static const Vec3 Red;
	static const Vec3 Orange;
	static const Vec3 DeepPink;
	static const Vec3 Pink;
	
	static const Vec3 Green;
	static const Vec3 ChartreuseGreen;
	static const Vec3 SpringGreen;
	static const Vec3 LightGreen;
	
	static const Vec3 Blue;
	static const Vec3 Violet;
	static const Vec3 AzureBlue;
	static const Vec3 LightBlue;
	
	static const Vec3 Yellow;
	static const Vec3 Magenta;
	static const Vec3 Teal;

	static const Vec3 Gold;
	static const Vec3 Transparent;

	double x{};
	double y{};
	double z{};

	Vec3() = default;
	Vec3(const double x, const double y, const double z) : x(x), y(y), z(z) {}
	Vec3(const double v) : x(v), y(v), z(v) {}
	double length() const;
	double distance(const Vec3& p) const;
	double dot(const Vec3& p);
	Vec3 maximum(double max) const;
	Vec3 minimum(double min) const;
	Vec3 clamp(double min, double max) const;
	Vec3 clamp(Vec3 min, Vec3 max) const;
	Vec3 saturate() const;
	Vec3 cross(const Vec3& p);
	Vec3 lerp(Vec3 v, double weight) const;
	void normalize();
	Vec3 rotateXAxis(const double rads) const ;
	Vec3 rotateYAxis(const double rads) const ;
	Vec3 rotateZAxis(const double rads) const ;
	Vec3 getNormal() const ;
	Vec3 quantize(size_t level);
	Vec3 floor() const;
	Vec3 ceil() const;
	Vec3 round() const;
	Vec3 abs() const;
	Vec3 operator-() const;
	Vec3 operator-(const Vec3& p) const;
	Vec3 operator*(const Vec3& p) const;
	Vec3 operator/(const Vec3& p) const;
	Vec3 operator+(const Vec3& p) const;
	Vec3& operator=(const Vec3& p);
	Vec3& operator+=(const Vec3& p);
	Vec3& operator-=(const Vec3& p);
	Vec3& operator*=(const Vec3& p);
	Vec3& operator/=(const Vec3& p);
	Vec3 operator+(const double d) const;
	Vec3 operator-(const double d) const;
	Vec3 operator*(const double d) const;
	Vec3 operator/(const double d) const;
	Vec3& operator=(const double d);
	Vec3& operator+=(const double d);
	Vec3& operator-=(const double d);
	Vec3& operator*=(const double d);
	Vec3& operator/=(const double d);
	bool operator==(const Vec3& p) const;
	bool operator==(double d) const;
	explicit operator bool() const;
};

std::ostream& operator<<(std::ostream& os, const Vec3& p);

	

using Color = Vec3;