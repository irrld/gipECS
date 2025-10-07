//
// Created by Metehan Gezer on 07/10/2025.
//

#ifndef GECS_INPUTMANAGER_H
#define GECS_INPUTMANAGER_H

#include "ecs/KeyCode.h"
#include "gBaseComponent.h"
#include "gWindowEvents.h"

#include <string>

namespace gecs {

struct KeyData {
	KeyData() { pressed = false; }
	explicit KeyData(bool pressed) : pressed(pressed) { }

	bool pressed;
};

enum InputMode {
	INPUTMODE_KEYBOARD_AND_MOUSE,
	INPUTMODE_GAMEPAD
};

class InputManager {
public:
	static bool getKey(const std::string& key);
	static bool isPressed(KeyCode keyCode);
	static float getAxis(const std::string& axisName);

	static int getMouseX();
	static int getMouseY();
	static gCursorMode getCursorMode();
	static void setCursorMode(gCursorMode mode);

	static void init();

private:

};

}

#endif//GECS_INPUTMANAGER_H
