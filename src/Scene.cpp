//
// Created by Metehan Gezer on 14/09/2025.
//

#include "ecs/Scene.h"
#include "ecs/InputManager.h"
#include "ecs/Behavior.h"
#include "ecs/Entity.h"

namespace gecs {

SceneCanvas::SceneCanvas(gBaseApp* app) : gBaseCanvas(app) {
	scene = std::make_unique<Scene>();
	InputManager::init();
}

SceneCanvas::~SceneCanvas() {}

void SceneCanvas::setup() {
	renderpassno = 1;
}

void SceneCanvas::update() {
	deltatime = appmanager->getElapsedTime();
	if (scene) {
		scene->update(deltatime);
		scene->processDestroyQueue();
	}
}

void SceneCanvas::draw() {
	if (scene) {
		scene->draw(deltatime);
	}
}

void SceneCanvas::onEvent(gEvent& event) {
	if (scene) {
		scene->onEvent(event);
	}
}

Scene::Scene() {
	bindSystem<Entity, BehaviorsComponent>(SystemType::UPDATE, G_BIND_FUNCTION(updateBehaviors));
	bindSystem<Entity, TransformComponent, CameraComponent>(SystemType::UPDATE, G_BIND_FUNCTION(updateCamera));
	bindSystem<Entity, TransformComponent, LightAmbientComponent>(SystemType::UPDATE, G_BIND_FUNCTION(updateLight));
	bindSystem<Entity, TransformComponent, ModelComponent>(SystemType::UPDATE, G_BIND_FUNCTION(updateModel));
	bindSystem<Entity, TransformComponent>(SystemType::UPDATE, G_BIND_FUNCTION(updateTransform));
	bindSystem<Entity, TransformComponent, ModelComponent>(SystemType::DRAW3D, G_BIND_FUNCTION(renderModel));
	bindSystem<Entity, TransformComponent, SpriteComponent>(SystemType::DRAW2D, G_BIND_FUNCTION(renderSprite));
}

Scene::~Scene() {

}

Entity Scene::createEntity(const std::string& name) {
	return createEntityWithUUID(gUUID(), name);
}

Entity Scene::createEntityWithUUID(gUUID uuid, const std::string& name) {
	Entity entity = {registry.create(), this};
	entity.addComponent<IdComponent>(uuid);
	entity.addComponent<TransformComponent>();
	entity.addComponent<TagComponent>(name.empty() ? "Entity" : name);

	entities[uuid] = entity;
	scenehierarchy.push_back(entity);
	return entity;
}

void Scene::removeEntity(Entity entity) {
	entities.erase(entity.getUUID());
	destroyqueue.push_back(entity.getHandle());
	scenehierarchy.erase(std::remove_if(scenehierarchy.begin(), scenehierarchy.end(), [&](auto& e) {
  return e == entity;
}));
}

void Scene::onUpdate(float_t deltatime) {

}

void Scene::onEvent(gEvent& event) {
	for (entt::entity entity : registry.view<BehaviorsComponent>()) {
		BehaviorsComponent& component = registry.get<BehaviorsComponent>(entity);
		component.onEvent(event);
	}
}

template<>
void Scene::onAddComponent(entt::entity entity, LightAmbientComponent& component) {
	component.data.enable();
}

template<>
void Scene::onRemoveComponent(entt::entity entity, LightAmbientComponent& component) {
	component.data.disable();
}

template<>
void Scene::onAddComponent(entt::entity entity, CameraComponent& component) {
	component.fbo.allocate(renderer->getScreenWidth(), renderer->getScreenHeight());
}

template<>
void Scene::onAddComponent(entt::entity entity, TransformComponent& component) {
	updateMatrices(entity);
}

void Scene::linkEntities(entt::entity parent, entt::entity child) {
	entt::entity loop_entity = parent;
	while (loop_entity != entt::null) {
		if (loop_entity == child) {
			return;
		}
		if (!registry.any_of<TreeComponent>(loop_entity)) {
			break;
		}
		auto& tree = registry.get_or_emplace<TreeComponent>(loop_entity);
		loop_entity = tree.parent;
	}
	auto& parent_tree = registry.get_or_emplace<TreeComponent>(parent);
	auto& child_tree = registry.get_or_emplace<TreeComponent>(child);
	if (child_tree.parent != entt::null) {
		unlinkEntities(child_tree.parent, child);
	}
	parent_tree.childs.push_back(child);
	child_tree.parent = parent;
	auto& child_transform = registry.get<TransformComponent>(child);
	auto& parent_transform = registry.get<TransformComponent>(parent);
	glm::vec3 posDiff = child_transform.position - parent_transform.position;
	glm::vec3 rotDiff = child_transform.rotation - parent_transform.rotation;

	child_transform.position = posDiff;
	child_transform.rotation = rotDiff;
	child_transform.ischanged = true;
}

void Scene::unlinkEntities(entt::entity parent, entt::entity child) {
	auto& parent_tree = registry.get_or_emplace<TreeComponent>(parent);
	auto& child_tree = registry.get_or_emplace<TreeComponent>(child);
	if (child_tree.parent == entt::null) {
		return;
	}
	parent_tree.childs.erase(
	 std::remove(parent_tree.childs.begin(), parent_tree.childs.end(), child),
	 parent_tree.childs.end());
	child_tree.parent = entt::null;
	auto& child_transform = registry.get<TransformComponent>(child);
	auto& parent_transform = registry.get<TransformComponent>(parent);
	glm::vec3 pos_diff = child_transform.position + parent_transform.position;
	glm::vec3 rot_diff = child_transform.rotation + parent_transform.rotation;

	child_transform.position = pos_diff;
	child_transform.rotation = rot_diff;
	child_transform.ischanged = true;
}

void Scene::processDestroyQueue() {
	for (const auto& item : destroyqueue) {
		destroyEntity(item);
	}
	destroyqueue.clear();
}

void Scene::update(float deltatime) {
	if (firstupdate) [[unlikely]] {
		/*for (entt::entity entity : registry.view<TransformComponent>()) {
			updateMatrices(entity);
		}*/
		firstupdate = false;
	}
	for (auto&& fn : systems[SystemType::UPDATE]) {
		fn(deltatime);
	}
}

void Scene::draw(float deltatime) {
	const auto it = registry.view<CameraComponent, TransformComponent>();
	// Render to each camera
	for (entt::entity handle : it) {
		CameraComponent& component = it.get<CameraComponent>(handle);
		if (!component.isenabled) {
			continue;
		}
		TransformComponent& transform = it.get<TransformComponent>(handle);
		component.fbo.bind();
		renderer->enableDepthTest();
		renderer->clearColor(0.0f, 0.0f, 0.0f, 0.0f);
		component.begin(transform.position, transform.transformmatrix);
		if (hasskybox) {
			skybox.draw();
		}
		for (auto&& fn : systems[SystemType::DRAW3D]) {
			fn(deltatime);
		}
		component.end();
		renderer->disableDepthTest();
		component.fbo.unbind();
	}
	renderpassno = renderpassnum - 1;

	// Render to screen
	renderer->bindDefaultFramebuffer();
	renderer->clearColor(0.0f, 0.0f, 0.0f, 1.0f);
	for (entt::entity handle : it) {
		CameraComponent& component = it.get<CameraComponent>(handle);
		if (!component.isenabled) {
			continue;
		}
		component.fbo.draw(0, 0, renderer->getWidth(), renderer->getHeight());
		break;
	}
	for (auto&& fn : systems[SystemType::DRAW2D]) {
		fn(deltatime);
	}
}

void Scene::setSkybox(std::shared_ptr<AssetBase> asset) {
	std::shared_ptr<SkyboxAsset> skyboxasset = std::reinterpret_pointer_cast<SkyboxAsset>(asset);
	if (skyboxasset) {
		skyboxasset->applyTo(skybox);
		hasskybox = true;
	}
}

bool Scene::onWindowResizeEvent(gWindowResizeEvent& event) {
	return false;
}

glm::mat4 Scene::makeLocal(const TransformComponent& t) {
	glm::vec3 rotRad = glm::radians(t.rotation);
	glm::mat4 R = glm::toMat4(glm::quat(rotRad));
	glm::mat4 T = glm::translate(glm::mat4(1.0f), t.position);
	glm::mat4 Tp = glm::translate(glm::mat4(1.0f), t.pivot);
	glm::mat4 Tn = glm::translate(glm::mat4(1.0f), -t.pivot);
	glm::mat4 S = glm::scale(glm::mat4(1.0f), t.scale);

	// move to Position, shift to Pivot, rotate+scale, shift back
	return T * Tp * R * S * Tn;
}

glm::mat4 Scene::getWorldMatrix(entt::entity entity) {
	auto& transform = registry.get<TransformComponent>(entity);
	glm::mat4 local = makeLocal(transform);

	if (auto* tree = registry.try_get<TreeComponent>(entity);
		tree && tree->parent != entt::null) {
		return getWorldMatrix(tree->parent) * local;
		}
	return local;
}

void Scene::updateMatrices(entt::entity entity) {
	auto& tc = registry.get<TransformComponent>(entity);
	tc.transformmatrix = getWorldMatrix(entity);
	tc.normalmatrix = glm::inverseTranspose(glm::mat3(tc.transformmatrix));
}

void Scene::destroyEntity(entt::entity handle) {
	registry.destroy(handle);
}

void Scene::updateTransform(float deltatime, Entity entity, TransformComponent& transform) {
	if (transform.ischanged) {
		updateMatrices(entity.getHandle());
		transform.ischanged = false;
	}
}

void Scene::updateCamera(float deltatime, Entity entity, TransformComponent& transform, CameraComponent& camera) {
	if (transform.ischanged) {
		updateMatrices(entity.getHandle());
		camera.data.setComponentsUnsafe(transform.position, transform.rotation, transform.scale, transform.transformmatrix);
		transform.ischanged = false;
	}
}

void Scene::updateModel(float deltatime, Entity entity, TransformComponent& transform, ModelComponent& model) {
	if (transform.ischanged) {
		updateMatrices(entity.getHandle());
		model.data.setPosition(transform.position);
		glm::quat orientation = glm::quatLookAt(glm::vec3(0.0f, 0.0f, -1.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)) *
				glm::angleAxis(transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)) *
					glm::angleAxis(transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) *
						glm::angleAxis(transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model.data.setOrientation(orientation);
		model.data.setTransformationMatrix(transform.transformmatrix);
		transform.ischanged = false;
	}
}

void Scene::updateLight(float deltatime, Entity entity, TransformComponent& transform, LightAmbientComponent& light) {
	if (light.isenabled && !light.data.isEnabled()) {
		light.data.enable();
	} else if (!light.isenabled && light.data.isEnabled()) {
		light.data.disable();
	}
}

void Scene::renderSprite(float deltatime, Entity entity, TransformComponent& transform, SpriteComponent& sprite) {
	float viewportheight = renderer->getHeight();
	float height = transform.scale.y * sprite.data.getHeight();
	float width = transform.scale.x * sprite.data.getWidth();
	sprite.data.draw(transform.position.x + transform.pivot.x, viewportheight - transform.position.y + transform.pivot.y - height,
		width, height, transform.pivot.x, transform.pivot.y, transform.rotation.x);
}

void Scene::renderModel(float deltatime, Entity entity, TransformComponent& transform, ModelComponent& model) {
	model.data.draw();
}

void Scene::updateBehaviors(float deltatime, Entity entity, BehaviorsComponent& behavior) {
	behavior.onUpdate(entity, deltatime);
}

}
