#include "InputManager.h"
#include <iostream>

InputManager& InputManager::instance()
{
	static InputManager instance{};
	return instance;
}

InputManager::InputManager()
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
	m_input = GetStdHandle(STD_INPUT_HANDLE);
}

void InputManager::processKeyPresses()
{
	for (auto& pair : m_keyMap) {
		pair.second.justPushed = false;
		pair.second.justReleased = false;
	}

	DWORD numEvents = 0;
	INPUT_RECORD inputBuffer[32];
	GetNumberOfConsoleInputEvents(m_input, &numEvents);

	if (numEvents > 0) {
		ReadConsoleInput(m_input, inputBuffer, 32, &numEvents);

		for (DWORD i = 0; i < numEvents; ++i) {
			if (inputBuffer[i].EventType == KEY_EVENT) {
				KEY_EVENT_RECORD ker = inputBuffer[i].Event.KeyEvent;

				if (ker.bKeyDown) {
					if (m_keyMap.contains(ker.wVirtualKeyCode)) {
						InputManager::KeyState& ks = m_keyMap.at(ker.wVirtualKeyCode);
						ks.justPushed = !ks.isDown;
						ks.justReleased = false;
						ks.isDown = true;
					}
					else {
						m_keyMap.insert({ ker.wVirtualKeyCode, { true, false, true } });
					}
				}
				else {
					if (m_keyMap.contains(ker.wVirtualKeyCode)) {
						InputManager::KeyState& ks = m_keyMap.at(ker.wVirtualKeyCode);
						ks.justPushed = false;
						ks.justReleased = ks.isDown;
						ks.isDown = false;
					}
					else {
						m_keyMap.insert({ ker.wVirtualKeyCode, { false, true, false } });
					}
				}
			}
		}
	}
}

void InputManager::processMouseMovement()
{
	static bool s_skipFrame{ true };
	if (GetForegroundWindow() == m_console) {
		if (s_skipFrame) {
			s_skipFrame = false;
		}
		else {
			POINT cursorPos;
			if (GetCursorPos(&cursorPos)) {
				RECT rect;
				GetClientRect(m_console, &rect);

				if (ScreenToClient(m_console, &cursorPos)) {
					double x = (SHORT)cursorPos.x;
					double y = (SHORT)cursorPos.y;

					m_mouseMotion = { x - m_screenCentre.x, y - m_screenCentre.y };
				}
			}
		}
		SetCursorPos(m_screenCentre.x, m_screenCentre.y);
	}
	else {
		s_skipFrame = true;
	}
}

void InputManager::update()
{
	instance().processMouseMovement();
	instance().processKeyPresses();
}

bool InputManager::keyJustPushed(unsigned short key)
{
	InputManager& im = instance();
	if (im.m_keyMap.contains(key)) {
		return im.m_keyMap.at(key).justPushed;
	}
	return false;
}

bool InputManager::keyJustReleased(unsigned short key)
{
	InputManager& im = instance();
	if (im.m_keyMap.contains(key)) {
		return im.m_keyMap.at(key).justReleased;
	}
	return false;
}

bool InputManager::keyDown(unsigned short key)
{
	InputManager& im = instance();
	if (im.m_keyMap.contains(key)) {
		return im.m_keyMap.at(key).isDown;
	}
	return false;
}

Vec2 InputManager::mouseMotion()
{
	return instance().m_mouseMotion;
}
