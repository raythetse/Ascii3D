#include "Mat3.h"
#include <format>
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
bool Mat3::operator==(const Mat3& m) const {
    for (size_t row{}; row < 3; ++row) {
        for (size_t col{}; col < 3; ++col) {
            if (m_mat[row][col] != m.m_mat[row][col]) return false;
        }
    }
    return true;
}

std::string Mat3::getShaderMat3()
{
    return std::format("mat3({:.7f}, {:.7f}, {:.7f}, {:.7f}, {:.7f}, {:.7f}, {:.7f}, {:.7f}, {:.7f})"
        , static_cast<float>(m_mat[0][0])
        , static_cast<float>(m_mat[0][1])
        , static_cast<float>(m_mat[0][2])
        , static_cast<float>(m_mat[1][0])
        , static_cast<float>(m_mat[1][1])
        , static_cast<float>(m_mat[1][2])
        , static_cast<float>(m_mat[2][0])
        , static_cast<float>(m_mat[2][1])
        , static_cast<float>(m_mat[2][2])
    );
}

void Mat3::fromAxisAngle(Vec3 axis, Mat3& mat, const double rads)
{
    Vec2 sc = MF::fast_sincos(rads);
    double omc = 1.0f - sc.y;

    axis.normalize();

    mat.m_mat[0][0] = axis.x * axis.x * omc + sc.y;
    mat.m_mat[0][1] = axis.x * axis.y * omc - axis.z * sc.x;
    mat.m_mat[0][2] = axis.x * axis.z * omc + axis.y * sc.x;
    mat.m_mat[1][0] = axis.y * axis.x * omc + axis.z * sc.x;
    mat.m_mat[1][1] = axis.y * axis.y * omc + sc.y;
    mat.m_mat[1][2] = axis.y * axis.z * omc - axis.x * sc.x;
    mat.m_mat[2][0] = axis.z * axis.x * omc - axis.y * sc.x;
    mat.m_mat[2][1] = axis.z * axis.y * omc + axis.x * sc.x;
    mat.m_mat[2][2] = axis.z * axis.z * omc + sc.y;
}
