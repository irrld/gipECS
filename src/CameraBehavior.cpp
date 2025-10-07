//
// Created by Metehan Gezer on 07/10/2025.
//

#include "ecs/behaviors/CameraBehavior.h"
#include "ecs/InputManager.h"

namespace gecs {

void CameraBehavior::onStart() {
	transform = entity.getComponentRef<TransformComponent>();
}

void CameraBehavior::onUpdate(float deltatime) {
	if (!transform) {
		return; // Transform component no longer exists
	}
	float axisx = InputManager::getAxis("Horizontal");
	float axisy = InputManager::getAxis("Vertical");
	transform->move(transform->getForward() * deltatime * movespeed * axisy);
	transform->move(transform->getRight() * deltatime * movespeed * axisx);
	float inputx = InputManager::getAxis("Mouse X");
	float inputy = InputManager::getAxis("Mouse Y");
	transform->setRotation(glm::vec3(inputy, inputx, 0.0f));
}

bool CameraBehavior::onKeyPressed(KeyCode key) {
	if (key == KEY_ESCAPE) {
		if (appmanager->getCursorMode() == CURSORMODE_RELATIVE) {
			appmanager->setCursorMode(CURSORMODE_NORMAL);
		} else {
			appmanager->setCursorMode(CURSORMODE_RELATIVE);
		}
	}
	return false;
}


}