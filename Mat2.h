#pragma once
#include <vector>
#include "Vec2.h"

class Mat2
{
public:
	static const Mat2 Identity;
	double m_mat[2][2]{ };
	Mat2() = default;
	Mat2(std::vector<double> values);
	Mat2 operator*(const Mat2& b) const;
	Vec2 operator*(const Vec2& b) const;
};

