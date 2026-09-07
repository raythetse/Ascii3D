#include "Mat2.h"
#include "MathFunctions.h"

const Mat2 Mat2::Identity{ std::vector<double>{ 1.0, 0.0, 0.0, 1.0 } };

Mat2::Mat2(std::vector<double> values)
{
    for (size_t i{}; i < values.size(); ++i) {
        m_mat[i % 2][i / 2] = values[i];
    }
}

Mat2 Mat2::operator*(const Mat2& other) const
{
    Mat2 result;

    for (size_t row{}; row < 2; ++row) {
        for (size_t col{}; col < 2; ++col) {
            result.m_mat[row][col] = m_mat[row][0] * other.m_mat[0][col] +
                m_mat[row][1] * other.m_mat[1][col];
        }
    }
    return result;
}

Vec2 Mat2::operator*(const Vec2& p) const {
    Vec2 result;

    result.x = m_mat[0][0] * p.x + m_mat[0][1] * p.y;
    result.y = m_mat[1][0] * p.x + m_mat[1][1] * p.y;

    return result;
}
