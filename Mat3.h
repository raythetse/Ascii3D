#pragma once
#include <vector>
#include "Vec3.h"

class Mat3
{
public:
	static const Mat3 Identity;
	double m_mat[3][3]{ };
	Mat3() = default;
	Mat3(std::vector<double> values);
	Mat3 operator*(const Mat3& b) const;
	Vec3 operator*(const Vec3& b) const;
	static Mat3 fromAxisAngle(Vec3 axis, const double rads);
};

