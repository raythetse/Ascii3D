#pragma once
#include <unordered_map>
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
	Vec2 m_mouseMotion{};
	HWND m_console;
	HANDLE m_input;
	POINT m_screenCentre;

	InputManager();
	~InputManager() {}

	void processKeyPresses();
	void processMouseMovement();

public:
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;
	InputManager& operator=(InputManager&&) = delete;

	static void update();
	static bool keyJustPushed(unsigned short key);
	static bool keyJustReleased(unsigned short key);
	static bool keyDown(unsigned short key);
	static Vec2 mouseMotion();
};
