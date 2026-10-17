#pragma once
#include <vector>
#include "Vec2.h"

class Mat2
{
public:
	static const Mat2 Identity;
	double m_mat[2][2]{ };
	Mat2() = default;
	Mat2(double v00, double v01, double v10, double v11);
	Mat2 operator*(const Mat2& b) const;
	Vec2 operator*(const Vec2& b) const;
};

