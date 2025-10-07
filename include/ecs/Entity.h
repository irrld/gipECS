//
// Created by Metehan Gezer on 14/09/2025.
//

#ifndef GIPECS_GENTITY_H
#define GIPECS_GENTITY_H

#include "ecs/Ref.h"
#include "ecs/Scene.h"
#include "ecs/Components.h"

namespace gecs {

class Entity {
public:
	Entity(entt::entity handle, Scene* scene);
	~Entity();

	template<typename T, typename... Args>
	T& addComponent(Args&&... args) {
		return scene->addComponent<T>(entityhandle, std::forward<Args>(args)...);
	}

	template<typename T>
	T& getComponent() {
		return scene->getComponent<T>(entityhandle);
	}


	template<typename T>
	ComponentRef<T> getComponentRef() {
		return scene->getComponentRef<T>(entityhandle);
	}

	template<typename T>
	bool hasComponent() const {
		return scene->hasComponent<T>(entityhandle);
	}

	template<typename T>
	void removeComponent() {
		scene->removeComponent<T>(entityhandle);
	}

	bool isValid() const {
		return entityhandle != entt::null && scene;
	}

	operator bool() const { return isValid(); }

	operator entt::entity() const { return entityhandle; }

	operator uint32_t() const { return (uint32_t) entityhandle; }

	gUUID getUUID() {
		return getComponent<IdComponent>().id;
	}

	const std::string& getName() {
		return getComponent<TagComponent>().tag;
	}
	Entity getParent() const { return {parenthandle, scene}; }
	entt::entity getParentHandle() const { return parenthandle; }
	const std::vector<entt::entity>* getChildHandles() const { return childhandles; }

	bool operator==(const Entity& other) const {
		return entityhandle == other.entityhandle && scene == other.scene;
	}

	bool operator!=(const Entity& other) const { return !(*this == other); }

	entt::entity getHandle() const { return entityhandle; }

	Scene* getScene() const { return scene; }

private:
	entt::entity entityhandle;
	Scene* scene;
	entt::entity parenthandle;
	std::vector<entt::entity>* childhandles;
};

}

#endif//GIPECS_GENTITY_H
