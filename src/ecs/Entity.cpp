//
// Created by Metehan Gezer on 14/09/2025.
//

#include "Entity.h"
#include "Scene.h"

namespace gecs {

Entity::Entity(entt::entity handle, Scene* scene)
	: entityhandle(handle), scene(scene) {
	childhandles = nullptr;
	parenthandle = entt::null;
	if (hasComponent<TreeComponent>()) {
		TreeComponent& component = getComponent<TreeComponent>();
		parenthandle = component.parent;
		childhandles = &component.childs;
	}
}

Entity::~Entity() {}

}
