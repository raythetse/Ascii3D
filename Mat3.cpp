#include "Mat3.h"
#include "MathFunctions.h"

const Mat3 Mat3::Identity{ std::vector<double>{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0} };

Mat3::Mat3(std::vector<double> values)
{
    for (size_t i{}; i < values.size(); ++i) {
        m_mat[i % 3][i / 3] = values[i];
    }
}

Mat3 Mat3::operator*(const Mat3& other) const
{
	Mat3 result;

	for (size_t row{}; row < 3; ++row) {
		for (size_t col{}; col < 3; ++col) {
			result.m_mat[row][col] = m_mat[row][0] * other.m_mat[0][col] + m_mat[row][1] * other.m_mat[1][col] + m_mat[row][2] * other.m_mat[2][col];
		}
	}
	return result;
}

Vec3 Mat3::operator*(const Vec3& p) const {
    Vec3 result;

    result.x = m_mat[0][0] * p.x + m_mat[0][1] * p.y + m_mat[0][2] * p.z;
    result.y = m_mat[1][0] * p.x + m_mat[1][1] * p.y + m_mat[1][2] * p.z;
    result.z = m_mat[2][0] * p.x + m_mat[2][1] * p.y + m_mat[2][2] * p.z;

    return result;
}

Mat3 Mat3::fromAxisAngle(Vec3 axis, const double rads)
{
    Mat3 result;
    Vec2 sc = MF::fast_sincos(rads);
    double omc = 1.0f - sc.y;

    axis.normalize();

    result.m_mat[0][0] = axis.x * axis.x * omc + sc.y;
    result.m_mat[0][1] = axis.x * axis.y * omc - axis.z * sc.x;
    result.m_mat[0][2] = axis.x * axis.z * omc + axis.y * sc.x;
    result.m_mat[1][0] = axis.y * axis.x * omc + axis.z * sc.x;
    result.m_mat[1][1] = axis.y * axis.y * omc + sc.y;
    result.m_mat[1][2] = axis.y * axis.z * omc - axis.x * sc.x;
    result.m_mat[2][0] = axis.z * axis.x * omc - axis.y * sc.x;
    result.m_mat[2][1] = axis.z * axis.y * omc + axis.x * sc.x;
    result.m_mat[2][2] = axis.z * axis.z * omc + sc.y;

    return result;
}
