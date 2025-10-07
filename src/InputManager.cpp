//
// Created by Metehan Gezer on 07/10/2025.
//

#include "ecs/InputManager.h"

#include "ecs/MouseCode.h"
#include "gAppManager.h"
#include "gRenderObject.h"
#include "gWindowEvents.h"

#include <map>

namespace gecs {

static std::map<std::string, std::vector<KeyCode>> keyboard_mapping_;
static std::map<KeyCode, KeyData> keys_;
static std::map<MouseCode, KeyData> mouse_buttons_;
static std::map<std::string, float> axis_;
static int mouse_x_ = 0;
static int mouse_y_ = 0;
static float mouse_axis_sens_x_ = 80.0f;
static float mouse_axis_sens_y_ = 80.0f;
static InputMode input_mode_ = INPUTMODE_KEYBOARD_AND_MOUSE;
static float mouse_axis_limit_y_ = 75.0f;

static void updateKeyboardAxis() {
	bool right = InputManager::getKey("Right");
	bool left = InputManager::getKey("Left");
	bool up = InputManager::getKey("Up");
	bool down = InputManager::getKey("Down");

	if (right && !left) {
		axis_["Horizontal"] = 1;
	} else if (!right && left) {
		axis_["Horizontal"] = -1;
	} else {
		axis_["Horizontal"] = 0;
	}

	if (up && !down) {
		axis_["Vertical"] = 1;
	} else if (!up && down) {
		axis_["Vertical"] = -1;
	} else {
		axis_["Vertical"] = 0;
	}
}

static bool onKeyPressed(gKeyPressedEvent& event) {
	input_mode_ = INPUTMODE_KEYBOARD_AND_MOUSE;
	keys_[event.getKeyCode()].pressed = true;
	updateKeyboardAxis();
	return false;
}

static bool onKeyReleased(gKeyReleasedEvent& event) {
	keys_[event.getKeyCode()].pressed = false;
	updateKeyboardAxis();
	return false;
}

static bool onMousePressed(gMouseButtonPressedEvent& event) {
	input_mode_ = INPUTMODE_KEYBOARD_AND_MOUSE;
	mouse_buttons_[event.getMouseButton()].pressed = true;
	updateKeyboardAxis();
	return false;
}

static bool onMouseReleased(gMouseButtonReleasedEvent& event) {
	mouse_buttons_[event.getMouseButton()].pressed = false;
	updateKeyboardAxis();
	return false;
}

static bool onMouseMoved(gMouseMovedEvent& event) {
	input_mode_ = INPUTMODE_KEYBOARD_AND_MOUSE;
	mouse_x_ = event.getX();
	mouse_y_ = event.getY();
	if (event.getCursorMode() == CURSORMODE_RELATIVE) {
		axis_["Mouse X"] += mouse_axis_sens_x_ * event.getX();
		axis_["Mouse Y"] += mouse_axis_sens_y_ * event.getY();
		axis_["Mouse Y"] = std::clamp(axis_["Mouse Y"], -mouse_axis_limit_y_, mouse_axis_limit_y_);
	}
	return false;
}

class Handler : public gBaseComponent {
public:
	void onEvent(gEvent& event) override {
		gEventDispatcher dispatcher{event};
		dispatcher.dispatch<gKeyPressedEvent>(G_BIND_GLOBAL_FUNCTION(onKeyPressed));
		dispatcher.dispatch<gKeyReleasedEvent>(G_BIND_GLOBAL_FUNCTION(onKeyReleased));
		dispatcher.dispatch<gMouseMovedEvent>(G_BIND_GLOBAL_FUNCTION(onMouseMoved));
		dispatcher.dispatch<gMouseButtonPressedEvent>(G_BIND_GLOBAL_FUNCTION(onMousePressed));
		dispatcher.dispatch<gMouseButtonReleasedEvent>(G_BIND_GLOBAL_FUNCTION(onMouseReleased));
	}
};

static std::unique_ptr<Handler> handler;

void InputManager::init() {
	if (handler) {
		return;
	}
	handler = std::make_unique<Handler>();
	keyboard_mapping_["Up"] = {KEY_ARROW_UP, KEY_W};
	keyboard_mapping_["Down"] = {KEY_ARROW_DOWN, KEY_S};
	keyboard_mapping_["Left"] = {KEY_ARROW_LEFT, KEY_A};
	keyboard_mapping_["Right"] = {KEY_ARROW_RIGHT, KEY_D};
	keyboard_mapping_["Jump"] = {KEY_SPACE};
	keyboard_mapping_["Enter"] = {KEY_ENTER};
	keyboard_mapping_["Left Shift"] = {KEY_LEFT_SHIFT};
	keyboard_mapping_["Right Shift"] = {KEY_RIGHT_SHIFT};
	keyboard_mapping_["Shift"] = {KEY_LEFT_SHIFT, KEY_RIGHT_SHIFT};
	keyboard_mapping_["Left Control"] = {KEY_LEFT_CONTROL};
	keyboard_mapping_["Right Control"] = {KEY_RIGHT_CONTROL};
	keyboard_mapping_["Control"] = {KEY_LEFT_CONTROL, KEY_RIGHT_CONTROL};
	keyboard_mapping_["Tab"] = {KEY_TAB};
	keyboard_mapping_["Return"] = {KEY_BACKSPACE};
}

bool InputManager::getKey(const std::string& key) {
	for (const auto& code : keyboard_mapping_[key]) {
		if (keys_[code].pressed) {
			return true;
		}
	}
	return false;
}

bool InputManager::isPressed(KeyCode code) {
	return keys_[code].pressed;
}

float InputManager::getAxis(const std::string& axisName) {
	return axis_[axisName];
}

int InputManager::getMouseX() {
	return mouse_x_;
}

int InputManager::getMouseY() {
	return mouse_y_;
}

gCursorMode InputManager::getCursorMode() {
	return appmanager->getCursorMode();
}

void InputManager::setCursorMode(gCursorMode mode) {
	appmanager->setCursorMode(mode);
}

}