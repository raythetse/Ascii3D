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

	acquireCentre();
	SetCursorPos(m_screenCentre.x, m_screenCentre.y);
	m_input = GetStdHandle(STD_INPUT_HANDLE);
	m_output = GetStdHandle(STD_OUTPUT_HANDLE);
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
			else if (inputBuffer[i].EventType == MOUSE_EVENT) {
				MOUSE_EVENT_RECORD mer = inputBuffer[i].Event.MouseEvent;
				m_holdMouse = m_holdMouse || (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED && mer.dwEventFlags == 0);
			}
			else if (inputBuffer[i].EventType == WINDOW_BUFFER_SIZE_EVENT) {
				COORD newSize = inputBuffer[i].Event.WindowBufferSizeEvent.dwSize;
				for (auto& func : m_resizeCallbacks) {
					func(newSize.X, newSize.Y - 1);
				}
			}
		}
	}
}

void InputManager::processMouseMovement()
{
	if (m_holdMouse) {
		if (m_skipFrame) {
			m_skipFrame = false;
		}
		else {
			POINT cursorPos;
			GetCursorPos(&cursorPos);
			//ScreenToClient(m_console, &cursorPos);
			m_mouseMotion = {
				static_cast<double>(cursorPos.x - m_screenCentre.x),
				static_cast<double>(cursorPos.y - m_screenCentre.y)
			};
		}
		//POINT conCentre{ m_screenCentre.x, m_screenCentre.y };
		//ClientToScreen(m_console, &conCentre);
		//SetCursorPos(conCentre.x, conCentre.y);
		SetCursorPos(m_screenCentre.x, m_screenCentre.y);
	}
	else {
		m_skipFrame = true;
	}
}

void InputManager::acquireCentre()
{
	RECT rect;
	if (GetClientRect(m_console, &rect)) {
		m_screenCentre.x = static_cast<long>(0.5 * (rect.right - rect.left));
		m_screenCentre.y = static_cast<long>(0.5 * (rect.bottom - rect.top));
	}
	m_skipFrame = true;
}

void InputManager::checkActive()
{
	m_isActiveWindow = GetForegroundWindow() == m_console;
	if (!m_isActiveWindow) m_holdMouse = false;
}

void InputManager::update()
{
	instance().checkActive();
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

void InputManager::registerResizeCallback(std::function<void(size_t, size_t)> callbackFunc)
{
	instance().m_resizeCallbacks.push_back(callbackFunc);
}
