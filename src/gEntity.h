//
// Created by Metehan Gezer on 14/09/2025.
//

#ifndef GIPECS_GENTITY_H
#define GIPECS_GENTITY_H

#include "gComponents.h"
#include "gScene.h"

class gEntity {
public:
	gEntity(entt::entity handle, gScene* scene);
	~gEntity();

	template<typename T, typename... Args>
	T& addComponent(Args&&... args) {
		return scene->addComponent<T>(entityhandle, std::forward<Args>(args)...);
	}

	template<typename T>
	T& getComponent() {
		return scene->getComponent<T>(entityhandle);
	}

	template<typename T>
	bool hasComponent() const {
		return scene->hasComponent<T>(entityhandle);
	}

	template<typename T>
	void removeComponent() {
		scene->removeComponent<T>(entityhandle);
	}

	operator bool() const { return entityhandle != entt::null; }

	operator entt::entity() const { return entityhandle; }

	operator uint32_t() const { return (uint32_t) entityhandle; }

	gUUID getUUID() {
		return getComponent<gIdComponent>().Id;
	}

	const std::string& getName() {
		return getComponent<gTagComponent>().tag;
	}
	gEntity getParent() const { return {parenthandle, scene}; }
	entt::entity getParentHandle() const { return parenthandle; }
	const std::vector<entt::entity>* getChildHandles() const { return childhandles; }

	bool operator==(const gEntity& other) const {
		return entityhandle == other.entityhandle && scene == other.scene;
	}

	bool operator!=(const gEntity& other) const { return !(*this == other); }

	entt::entity getHandle() const { return entityhandle; }

	gScene* getScene() const { return scene; }

private:
	entt::entity entityhandle;
	gScene* scene;
	entt::entity parenthandle;
	std::vector<entt::entity>* childhandles;
};


#endif//GIPECS_GENTITY_H
