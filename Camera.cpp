#define OEMRESOURCE
#include "Camera.h"
#include "Mat3.h"
#include "Vec2.h"

Camera::Camera(const double viewportWidth, const double viewportHeight, const Vec3 position, const double lookSpeed, const double moveSpeed)
    : m_viewportWidth(viewportWidth)
    , m_viewportHeight(viewportHeight)
    , m_position(position)
    , m_initialPosition(position)
    , m_lookSpeed(lookSpeed * 0.001)
    , m_moveSpeed(moveSpeed)
{
    m_console = GetConsoleWindow();
    HWND realWindow = GetAncestor(m_console, GA_ROOTOWNER);
    m_console = (realWindow != NULL) ? realWindow : m_console;

    RECT rect;
    if (GetClientRect(m_console, &rect)) {
        m_screenCentre.x = static_cast<long>(0.5 * (rect.right - rect.left));
        m_screenCentre.y = static_cast<long>(0.5 * (rect.bottom - rect.top));
    }
    SetCursorPos(m_screenCentre.x, m_screenCentre.y);
}

void Camera::update(double deltaTime)
{
    POINT cursorPos;

    if (GetCursorPos(&cursorPos)) {
        RECT rect;
        GetClientRect(m_console, &rect);

        if (ScreenToClient(m_console, &cursorPos)) {
            double x = (SHORT)cursorPos.x;
            double y = (SHORT)cursorPos.y;

            Vec2 diff{ x - m_screenCentre.x, y - m_screenCentre.y };
            //diff.normalize();

            if (diff.x != 0 || diff.y != 0) {
                m_rotation.x += diff.y * m_lookSpeed;
                m_rotation.x = (m_rotation.x < -1.57 ? -1.57 : m_rotation.x);
                m_rotation.x = (m_rotation.x > 1.57 ? 1.57 : m_rotation.x);
                m_rotation.y += diff.x * m_lookSpeed;

                Mat3 rotX = Mat3::fromAxisAngle({ 1.0, 0.0, 0.0 }, m_rotation.x);
                Mat3 rotY = Mat3::fromAxisAngle({ 0.0, 1.0, 0.0 }, m_rotation.y);
                Mat3 m = rotY * rotX;

                m_forward = m * Vec3{ 0.0, 0.0, 1.0 };
                m_up = m * Vec3{ 0.0, 1.0, 0.0 };
                m_right = m * Vec3{ 1.0, 0.0, 0.0 };
            }
        }
    }
    SetCursorPos(m_screenCentre.x, m_screenCentre.y);

    if (GetAsyncKeyState('A') & 0x8000) {
        Vec3 right = Vec3{ m_right.x, 0.0, m_right.z }.getNormal();
        m_position -= right * m_moveSpeed * deltaTime;
    }
    if (GetAsyncKeyState('D') & 0x8000) {
        Vec3 right = Vec3{ m_right.x, 0.0, m_right.z }.getNormal();
        m_position += right * m_moveSpeed * deltaTime;
    }
    if (GetAsyncKeyState('W') & 0x8000) {
        Vec3 forward = Vec3{ m_forward.x, 0.0, m_forward.z }.getNormal();
        m_position += forward * m_moveSpeed * deltaTime;
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        Vec3 forward = Vec3{ m_forward.x, 0.0, m_forward.z }.getNormal();
        m_position -= forward * m_moveSpeed * deltaTime;
    }
    if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) {
        m_position += Vec3::Down * m_moveSpeed * deltaTime;
    }
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        m_position += Vec3::Up * m_moveSpeed * deltaTime;
    }
}

void Camera::reset()
{
    m_position = m_initialPosition;
    m_right = Vec3::Right;
    m_up = Vec3::Up;
    m_forward = Vec3::Forward;
    m_rotation = Vec3::Zero;
}

Vec3 Camera::getViewportRay(const double x, const double y) const
{
    double scaledHeight = m_viewportHeight;

    Vec3 screenPoint = m_position + m_forward;
    screenPoint -= m_right * 0.5 * m_viewportWidth;
    screenPoint += m_right * m_viewportWidth * x;
    screenPoint += m_up * 0.5 * scaledHeight;
    screenPoint -= m_up * scaledHeight * y;

    return (screenPoint - m_position).getNormal();
}
