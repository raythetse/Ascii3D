#pragma once
#include <string>
#include <vector>
#include "Vec3.h"

class Mat3
{
public:
	static const Mat3 Identity;
	double m_mat[3][3]{ 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
	Mat3() = default;
	Mat3(std::vector<double> values);
	Mat3 operator*(const Mat3& b) const;
	Vec3 operator*(const Vec3& b) const;
	bool operator==(const Mat3& m) const;
	std::string getShaderMat3();
	static void fromAxisAngle(Vec3 axis, Mat3& mat, const double rads);
};

