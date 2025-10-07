//
// Created by Metehan Gezer on 07/10/2025.
//

#include "ecs/Behavior.h"
#include "ecs/InputManager.h"

#include <intrin.h>

namespace gecs {

void BehaviorsComponent::onUpdate(Entity entity, float deltatime) {
	if (firstupdate) {
		// This should be moved out
		for (auto&& pair : behaviors) {
			pair.second->entity = entity;
			pair.second->scene = entity.getScene();
			pair.second->onStart();
		}
		firstupdate = false;
	}
	for (auto&& pair : behaviors) {
		pair.second->onUpdate(deltatime);
	}
}

void BehaviorsComponent::onEvent(gEvent& event) {
	gEventDispatcher dispatcher{event};
	dispatcher.dispatch<gKeyPressedEvent>(G_BIND_FUNCTION(onKeyPressed));
	dispatcher.dispatch<gKeyReleasedEvent>(G_BIND_FUNCTION(onKeyReleased));
	dispatcher.dispatch<gMouseMovedEvent>(G_BIND_FUNCTION(onMouseMoved));
	for (auto&& pair : behaviors) {
		pair.second->onEvent(event);
	}
}

bool BehaviorsComponent::onKeyPressed(gKeyPressedEvent& event) {
	for (auto&& pair : behaviors) {
		if (pair.second->onKeyPressed(event.getKeyCode())) {
			return true;
		}
	}
	return false;
}

bool BehaviorsComponent::onKeyReleased(gKeyReleasedEvent& event) {
	for (auto&& pair : behaviors) {
		if (pair.second->onKeyReleased(event.getKeyCode())) {
			return true;
		}
	}
	return false;
}

bool BehaviorsComponent::onMouseMoved(gMouseMovedEvent& event) {
	for (auto&& pair : behaviors) {
		if (pair.second->onMouseMoved(event.getX(), event.getY(), event.getCursorMode())) {
			return true;
		}
	}
	return false;
}

}