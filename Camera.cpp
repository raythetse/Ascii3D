#define OEMRESOURCE
#include "Camera.h"
#include "GameTimer.h"
#include "InputManager.h"
#include "Mat3.h"
#include "Vec2.h"

void Camera::update()
{
    if (InputManager::keyDown(VK_BACK)) reset();

    Vec2 diff = InputManager::mouseMotion();;

    if (diff.x != 0 || diff.y != 0) {
        m_rotation.x += diff.y * m_lookSpeed;
        m_rotation.x = (m_rotation.x < -1.57 ? -1.57 : m_rotation.x);
        m_rotation.x = (m_rotation.x > 1.57 ? 1.57 : m_rotation.x);
        m_rotation.y += diff.x * m_lookSpeed;

        Mat3 rotX; Mat3::fromAxisAngle({ 1.0, 0.0, 0.0 }, rotX, m_rotation.x);
        Mat3 rotY; Mat3::fromAxisAngle({ 0.0, 1.0, 0.0 }, rotY, m_rotation.y);
        Mat3 m = rotY * rotX;
        m_forward = m * Vec3{ 0.0, 0.0, 1.0 };
        m_up = m * Vec3{ 0.0, 1.0, 0.0 };
        m_right = m * Vec3{ 1.0, 0.0, 0.0 };
    }

    double delta = GameTimer::delta();
    if (InputManager::keyDown('A')) {
        Vec3 right = Vec3{ m_right.x, 0.0, m_right.z }.getNormal();
        m_position -= right * m_moveSpeed * delta;
    }
    if (InputManager::keyDown('D')) {
        Vec3 right = Vec3{ m_right.x, 0.0, m_right.z }.getNormal();
        m_position += right * m_moveSpeed * delta;
    }
    if (InputManager::keyDown('W')) {
        Vec3 forward = Vec3{ m_forward.x, 0.0, m_forward.z }.getNormal();
        m_position += forward * m_moveSpeed * delta;
    }
    if (InputManager::keyDown('S')) {
        Vec3 forward = Vec3{ m_forward.x, 0.0, m_forward.z }.getNormal();
        m_position -= forward * m_moveSpeed * delta;
    }
    if (InputManager::keyDown(VK_SHIFT)) {
        m_position += Vec3::Down * m_moveSpeed * delta;
    }
    if (InputManager::keyDown(VK_SPACE)) {
        m_position += Vec3::Up * m_moveSpeed * delta;
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
    Vec3 screenPoint = m_position + m_forward;
    screenPoint -= m_right * 0.5 * m_viewportWidth;
    screenPoint += m_right * m_viewportWidth * x;
    screenPoint += m_up * 0.5 * m_viewportHeight;
    screenPoint -= m_up * m_viewportHeight * y;

    return (screenPoint - m_position).getNormal();
}
