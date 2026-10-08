#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <Windows.h>
#include "Vec2.h"

class InputManager
{
public:
	struct KeyState
	{
		bool justPushed;
		bool justReleased;
		bool isDown;
	};
private:
	static InputManager& instance();
	std::unordered_map<unsigned short, KeyState> m_keyMap{};
	std::vector<std::function<void(size_t, size_t)>> m_resizeCallbacks;
	Vec2 m_mouseMotion{};
	HWND m_console;
	HANDLE m_input;
	HANDLE m_output;
	POINT m_windowCentre;
	bool m_holdMouse{ true };
	bool m_isActiveWindow{ true };
	bool m_windowResized{ true };

	InputManager();
	~InputManager() {}

	void processKeyEvents(INPUT_RECORD inputRecord);
	void processMouseEvents(INPUT_RECORD inputRecord);
	void processWindowResizeEvents(INPUT_RECORD inputRecord);
	void processInputEvents();
	void processMouseMovement();
	void acquireCentre();
	void checkActive();

public:
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;
	InputManager& operator=(InputManager&&) = delete;

	static void reset();
	static void update();
	static bool keyJustPushed(unsigned short key);
	static bool keyJustReleased(unsigned short key);
	static bool keyDown(unsigned short key);
	static Vec2 mouseMotion();
	static void registerResizeCallback(std::function<void(size_t, size_t)> callbackFunc);
};
