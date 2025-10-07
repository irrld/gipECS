//
// Created by Metehan Gezer on 07/10/2025.
//

#ifndef GECS_BEHAVIOR_H
#define GECS_BEHAVIOR_H

#include "ecs/Components.h"
#include "ecs/Entity.h"
#include "ecs/KeyCode.h"
#include "ecs/Ref.h"
#include "ecs/Scene.h"

namespace gecs {

struct BehaviorBase {
	virtual ~BehaviorBase() {}
	virtual void onStart() {}
	virtual void onUpdate(float deltatime) {}

	virtual void onEvent(gEvent& event) {}
	virtual bool onKeyPressed(KeyCode keyCode) { return false; }
	virtual bool onKeyReleased(KeyCode keyCode) { return false; }
	virtual bool onMouseMoved(float x, float y, gCursorMode cursorMode) { return false; }
protected:
	friend class BehaviorsComponent;
	Entity entity{entt::null, nullptr};
	Scene* scene = nullptr;
};

struct BehaviorsComponent : public ComponentBase {
	void onUpdate(Entity entity, float deltatime);
	void onEvent(gEvent& event);
	bool onKeyPressed(gKeyPressedEvent& event);
	bool onKeyReleased(gKeyReleasedEvent& event);
	bool onMouseMoved(gMouseMovedEvent& event);

	template<typename T, typename... Args>
	T& addBehavior(Args&&... args) {
		behaviors[typeid(T)] = std::move(std::make_shared<T>(std::forward<Args>(args)...));
		return *getBehavior<T>();
	}

	template<typename T>
	void removeBehavior() {
		behaviors.erase(typeid(T));
	}

	template<typename T>
	bool hasBehavior() const {
		const auto it = behaviors.find(typeid(T));
		return it != behaviors.end();
	}

	template<typename T>
	T* getBehavior() {
		const auto it = behaviors.find(typeid(T));
		if (it != behaviors.end()) {
			return static_cast<T*>(it->second.get());
		}
		return nullptr;
	}

private:
	std::unordered_map<std::type_index, std::shared_ptr<BehaviorBase>> behaviors;
	bool firstupdate = true;
};

}

#endif//GECS_BEHAVIOR_H
