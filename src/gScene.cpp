//
// Created by Metehan Gezer on 14/09/2025.
//

#include "gScene.h"
#include "gEntity.h"

gSceneCanvas::gSceneCanvas(gBaseApp* app) : gBaseCanvas(app) {
}

gSceneCanvas::~gSceneCanvas() {}

void gSceneCanvas::setup() {
	renderpassno = 1;
}

void gSceneCanvas::update() {
	deltatime = appmanager->getElapsedTime();
	if (scene) {
		scene->tick(deltatime);
	}
}

void gSceneCanvas::draw() {
	if (scene) {
		scene->draw(deltatime);
	}
}

void gSceneCanvas::onEvent(gEvent& event) {
	if (scene) {
		scene->onEvent(event);
	}
}

gScene::gScene() {
	bindSystem<gEntity, gTransformComponent>(gSystem::TICK, G_BIND_FUNCTION(updateTransform));
	bindSystem<gEntity, gTransformComponent, gSpriteComponent>(gSystem::DRAW, G_BIND_FUNCTION(renderSprite));
}

gScene::~gScene() {

}

gEntity gScene::createEntity(const std::string& name) {
	return createEntityWithUUID(gUUID(), name);
}

gEntity gScene::createEntityWithUUID(gUUID uuid, const std::string& name) {
	gEntity entity = {registry.create(), this};
	entity.addComponent<gIdComponent>(uuid);
	entity.addComponent<gTransformComponent>();
	entity.addComponent<gTagComponent>(name.empty() ? "Entity" : name);

	entities[uuid] = entity;
	scenehierarchy.push_back(entity);
	return entity;
}

void gScene::removeEntity(gEntity entity) {
	entities.erase(entity.getUUID());
	destroyqueue.push_back(entity.getHandle());
	scenehierarchy.erase(std::remove_if(scenehierarchy.begin(), scenehierarchy.end(), [&](auto& e) {
  return e == entity;
}));
}

void gScene::onUpdate(float_t deltatime) {

}

void gScene::onEvent(gEvent& event) {

}

void gScene::linkEntities(entt::entity parent, entt::entity child) {
	entt::entity loop_entity = parent;
	while (loop_entity != entt::null) {
		if (loop_entity == child) {
			return;
		}
		if (!registry.any_of<gTreeComponent>(loop_entity)) {
			break;
		}
		auto& tree = registry.get_or_emplace<gTreeComponent>(loop_entity);
		loop_entity = tree.parent;
	}
	auto& parent_tree = registry.get_or_emplace<gTreeComponent>(parent);
	auto& child_tree = registry.get_or_emplace<gTreeComponent>(child);
	if (child_tree.parent != entt::null) {
		unlinkEntities(child_tree.parent, child);
	}
	parent_tree.childs.push_back(child);
	child_tree.parent = parent;
	auto& child_transform = registry.get<gTransformComponent>(child);
	auto& parent_transform = registry.get<gTransformComponent>(parent);
	glm::vec3 posDiff = child_transform.position - parent_transform.position;
	glm::vec3 rotDiff = child_transform.rotation - parent_transform.rotation;

	child_transform.position = posDiff;
	child_transform.rotation = rotDiff;
	child_transform.ischanged = true;
}

void gScene::unlinkEntities(entt::entity parent, entt::entity child) {
	auto& parent_tree = registry.get_or_emplace<gTreeComponent>(parent);
	auto& child_tree = registry.get_or_emplace<gTreeComponent>(child);
	if (child_tree.parent == entt::null) {
		return;
	}
	parent_tree.childs.erase(
	 std::remove(parent_tree.childs.begin(), parent_tree.childs.end(), child),
	 parent_tree.childs.end());
	child_tree.parent = entt::null;
	auto& child_transform = registry.get<gTransformComponent>(child);
	auto& parent_transform = registry.get<gTransformComponent>(parent);
	glm::vec3 pos_diff = child_transform.position + parent_transform.position;
	glm::vec3 rot_diff = child_transform.rotation + parent_transform.rotation;

	child_transform.position = pos_diff;
	child_transform.rotation = rot_diff;
	child_transform.ischanged = true;
}

void gScene::processDestroyQueue() {
	for (const auto& item : destroyqueue) {
		destroyEntity(item);
	}
	destroyqueue.clear();
}

void gScene::tick(float deltatime) {
	auto view = registry.view<entt::entity>();

	for (size_t i = 0; i < ticksystems.size(); ++i) {
		for (auto handle : view) {
			gEntity entity{handle, this};
			if (tickfilters[i](entity, registry)) {
				ticksystems[i](deltatime, entity, registry);
			}
		}
	}
}

void gScene::draw(float deltatime) {
	renderer->clear();

	auto view = registry.view<entt::entity>();

	for (size_t i = 0; i < drawsystems.size(); ++i) {
		for (auto handle : view) {
			gEntity entity{handle, this};
			if (drawfilters[i](entity, registry)) {
				drawsystems[i](deltatime, entity, registry);
			}
		}
	}

	renderpassno = renderpassnum - 1;
}

bool gScene::onWindowResizeEvent(gWindowResizeEvent& event) {
	return false;
}

glm::mat4 gScene::makeLocal(const gTransformComponent& t) {
	glm::vec3 rotRad = glm::radians(t.rotation);
	glm::mat4 R = glm::toMat4(glm::quat(rotRad));
	glm::mat4 T = glm::translate(glm::mat4(1.0f), t.position);
	glm::mat4 Tp = glm::translate(glm::mat4(1.0f), t.pivot);
	glm::mat4 Tn = glm::translate(glm::mat4(1.0f), -t.pivot);
	glm::mat4 S = glm::scale(glm::mat4(1.0f), t.scale);

	// move to Position, shift to Pivot, rotate+scale, shift back
	return T * Tp * R * S * Tn;
}
glm::mat4 gScene::getWorldMatrix(entt::entity entity) {
	auto& transform = registry.get<gTransformComponent>(entity);
	glm::mat4 local = makeLocal(transform);

	if (auto* tree = registry.try_get<gTreeComponent>(entity);
		tree && tree->parent != entt::null) {
		return getWorldMatrix(tree->parent) * local;
		}
	return local;

}
void gScene::updateMatrices(entt::entity entity) {
	auto& tc = registry.get<gTransformComponent>(entity);
	tc.transformmatrix = getWorldMatrix(entity);
	tc.normalmatrix = glm::inverseTranspose(glm::mat3(tc.transformmatrix));
}

void gScene::destroyEntity(entt::entity handle) {
	registry.destroy(handle);
}

void gScene::updateTransform(float deltatime, gEntity entity, gTransformComponent& transform) {
	if (transform.ischanged) {
		updateMatrices(entity.getHandle());
		transform.ischanged = false;
	}
}

void gScene::renderSprite(float deltatime, gEntity entity, gTransformComponent& transform, gSpriteComponent& sprite) {
	float viewportheight = renderer->getHeight();
	float height = transform.scale.y * sprite.image.getHeight();
	float width = transform.scale.x * sprite.image.getWidth();
	sprite.image.draw(transform.position.x + transform.pivot.x, viewportheight - transform.position.y + transform.pivot.y - height,
		width, height, transform.pivot.x, transform.pivot.y, transform.rotation.x);
}
