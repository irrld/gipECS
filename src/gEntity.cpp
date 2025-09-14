//
// Created by Metehan Gezer on 14/09/2025.
//

#include "gEntity.h"
#include "gScene.h"

gEntity::gEntity(entt::entity handle, gScene* scene)
	: entityhandle(handle), scene(scene) {
	childhandles = nullptr;
	parenthandle = entt::null;
	if (hasComponent<gTreeComponent>()) {
		gTreeComponent& component = getComponent<gTreeComponent>();
		parenthandle = component.parent;
		childhandles = &component.childs;
	}
}

gEntity::~gEntity() {}
