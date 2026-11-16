#include "InputManager.h"
#include <iostream>

InputManager& InputManager::instance()
{
	static InputManager instance{};
	return instance;
}

InputManager::InputManager()
{
	// Best guess at the host window. Under older Windows Terminal versions the pseudo-window's
	// owner is the Terminal window; since 1.25 it has none, and processMouseEvents learns the
	// real host from the first click.
	m_console = GetConsoleWindow();
	HWND realWindow = GetAncestor(m_console, GA_ROOTOWNER);
	m_console = (realWindow != NULL) ? realWindow : m_console;

	acquireCentre();
	SetCursorPos(m_windowCentre.x, m_windowCentre.y);
	m_input = GetStdHandle(STD_INPUT_HANDLE);
	m_output = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode = 0;
	if (GetConsoleMode(m_input, &mode)) {
		// Enable mouse input and disable QuickEdit mode
		mode |= ENABLE_MOUSE_INPUT;
		mode &= ~ENABLE_QUICK_EDIT_MODE;
		mode |= ENABLE_EXTENDED_FLAGS; // Required to change extended flags safely
		SetConsoleMode(m_input, mode);
	}
}

void InputManager::processKeyEvents(INPUT_RECORD inputRecord)
{
	KEY_EVENT_RECORD ker = inputRecord.Event.KeyEvent;

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

void InputManager::processMouseEvents(INPUT_RECORD inputRecord)
{
	MOUSE_EVENT_RECORD mer = inputRecord.Event.MouseEvent;
	if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED && !mer.dwEventFlags) {
		// A click only reaches us while the window hosting us has focus, so the foreground
		// window is that host. Under Windows Terminal, GetConsoleWindow() returns an invisible
		// pseudo-window, and since Terminal 1.25 its owner is no longer the Terminal window.
		HWND host = GetForegroundWindow();
		if (host != NULL && host != m_console) {
			m_console = host;
			m_hostChanged = true;
		}
		m_holdMouse = true;
	}
}

void InputManager::processFocusEvents(INPUT_RECORD inputRecord)
{
	// Covers switching to another tab of the same Terminal window, where the foreground
	// window doesn't change (if the host forwards focus events)
	if (!inputRecord.Event.FocusEvent.bSetFocus) m_holdMouse = false;
}

void InputManager::processWindowResizeEvents(INPUT_RECORD inputRecord)
{
	COORD newSize = inputRecord.Event.WindowBufferSizeEvent.dwSize;
	for (auto& func : m_resizeCallbacks) {
		func(newSize.X, newSize.Y - 1);
	}
}

void InputManager::processInputEvents()
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
			switch (inputBuffer[i].EventType)
			{
			case KEY_EVENT:
				processKeyEvents(inputBuffer[i]);
				break;
			case MOUSE_EVENT:
				processMouseEvents(inputBuffer[i]);
				break;
			case FOCUS_EVENT:
				processFocusEvents(inputBuffer[i]);
				break;
			case WINDOW_BUFFER_SIZE_EVENT:
				processWindowResizeEvents(inputBuffer[i]);
				m_windowResized = true;
				break;
			default:
				break;
			}
		}
	}
}

void InputManager::processMouseMovement()
{
	if (m_holdMouse) {
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		if (m_windowResized || m_hostChanged) {
			// The centre just moved; the cursor's offset from it isn't mouse motion
			m_mouseMotion = { 0.0 };
			m_windowResized = false;
			m_hostChanged = false;
		}
		else {
			m_mouseMotion = { static_cast<double>(cursorPos.x - m_windowCentre.x), static_cast<double>(cursorPos.y - m_windowCentre.y) };
		}
		SetCursorPos(m_windowCentre.x, m_windowCentre.y);
	}
}

void InputManager::acquireCentre()
{
	if (!m_windowResized && !m_hostChanged) return;

	RECT rect;
	if (GetWindowRect(m_console, &rect)) {
		m_windowCentre.x = static_cast<long>(rect.left + 0.5 * (rect.right - rect.left));
		m_windowCentre.y = static_cast<long>(rect.top + 0.5 * (rect.bottom - rect.top));
	}
}

void InputManager::checkActive()
{
	m_isActiveWindow = GetForegroundWindow() == m_console;
	if (!m_isActiveWindow) m_holdMouse = false;
}

void InputManager::reset()
{
	instance().m_keyMap.clear();
}

void InputManager::update()
{
	instance().acquireCentre();
	instance().checkActive();
	instance().processMouseMovement();
	instance().processInputEvents();
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
