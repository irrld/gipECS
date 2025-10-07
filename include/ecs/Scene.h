//
// Created by Metehan Gezer on 14/09/2025.
//

#ifndef GIPECS_GSCENE_H
#define GIPECS_GSCENE_H

#include "ecs/Components.h"
#include "ecs/Ref.h"
#include "ecs/System.h"
#include "gBaseCanvas.h"
#include "gUUID.h"

namespace gecs {
enum class SystemType {
	UPDATE,
	DRAW3D,
	DRAW2D
};

class Entity;
class BehaviorsComponent;

class Scene : gRenderObject {
public:
	Scene();
	~Scene();

	Entity createEntity(const std::string& name = std::string());
	Entity createEntityWithUUID(gUUID uuid,
								 const std::string& name = std::string());
	void removeEntity(Entity entity);

	void onUpdate(float_t deltatime);
	void onEvent(gEvent& event);

	template<typename T>
	void onAddComponent(entt::entity entity, T& component) {}
	template<typename T>
	void onRemoveComponent(entt::entity entity, T& component) {}
	template<>
	void onAddComponent(entt::entity entity, LightAmbientComponent& component);
	template<>
	void onRemoveComponent(entt::entity entity, LightAmbientComponent& component);
	template<>
	void onAddComponent(entt::entity entity, CameraComponent& component);
	template<>
	void onAddComponent(entt::entity entity, TransformComponent& component);

	template<typename T, typename... Args>
	T& addComponent(entt::entity handle, Args&&... args) {
		if (hasComponent<T>(handle)) {
			//throw std::runtime_error("Entity already has component!");
			return getComponent<T>(handle);
		}
		auto& component = registry.emplace<T>(
				handle, std::forward<Args>(args)...);
		onAddComponent(handle, component);
		return component;
	}

	template<typename T>
	T& getComponent(entt::entity handle) {
		return registry.get<T>(handle);
	}

	template<typename T>
	ComponentRef<T> getComponentRef(entt::entity handle) {
		return ComponentRef<T>(registry, handle);
	}

	template<typename T>
	bool hasComponent(entt::entity handle) const {
		return registry.any_of<T>(handle);
	}

	template<typename T>
	void removeComponent(entt::entity handle) {
		if (!hasComponent<T>(handle)) {
			return;
		}
		auto& component = getComponent<T>(handle);
		onRemoveComponent<T>(handle, component);
		registry.remove<T>(handle);
	}

	template<typename... Components>
	auto getAllEntitiesWith() {
		return registry.view<Components...>();
	}

	/*
   * Returns the scene hierarchy.
   */
	std::vector<entt::entity>& getSceneHierarchy() { return scenehierarchy; }

	void linkEntities(entt::entity parent, entt::entity child);
	void unlinkEntities(entt::entity parent, entt::entity child);

	void processDestroyQueue();

	template<typename Entity, typename... Components, typename Func>
	void bindSystem(SystemType type, Func func) {
		systems[type].push_back([this, func](float deltatime) {
					for (auto handle : registry.view<Components...>()) {
						Entity entity{handle, this};
						func(deltatime, entity, registry.get<Components>(handle)...);
					}
				});
	}

	template<typename Entity, typename... Components, typename Func>
	void bindSystem(SystemType type, const std::string& tag, Func func) {
		systems[type].push_back([this, func, tag](float deltatime) {
					for (auto handle : registry.view<Components...>()) {
						Entity entity{handle, this};
						if (entity.getName() != tag) {
							continue;
						}
						func(deltatime, entity, registry.get<Components>(handle)...);
					}
				});
	}

	void update(float deltatime);
	void draw(float deltatime);

private:
	bool onWindowResizeEvent(gWindowResizeEvent& event);
	glm::mat4 makeLocal(const TransformComponent& transform);
	glm::mat4 getWorldMatrix(entt::entity entity);
	void updateMatrices(entt::entity entity);
	void destroyEntity(entt::entity handle);

	void updateTransform(float deltatime, Entity entity, TransformComponent& transform);
	void updateCamera(float deltatime, Entity entity, TransformComponent& transform, CameraComponent& camera);
	void updateModel(float deltatime, Entity entity, TransformComponent& transform, ModelComponent& model);
	void updateLight(float deltatime, Entity entity, TransformComponent& transform, LightAmbientComponent& light);
	void renderSprite(float deltatime, Entity entity, TransformComponent& transform, SpriteComponent& sprite);
	void renderModel(float deltatime, Entity entity, TransformComponent& transform, ModelComponent& model);
	void updateBehaviors(float deltatime, Entity entity, BehaviorsComponent& behaviors);

private:
	friend class SceneCanvas;

	std::unordered_map<gUUID, entt::entity> entities;
	entt::registry registry;
	bool firstupdate = true;
	std::vector<entt::entity> scenehierarchy;
	std::vector<entt::entity> destroyqueue;

	std::unordered_map<SystemType, std::vector<std::function<void(float)>>> systems;
};

class SceneCanvas : public gBaseCanvas {
public:
	SceneCanvas(gBaseApp*);
	~SceneCanvas() override;
	void setup() override;
	void update() override;
	void draw() override;
	void onEvent(gEvent& event) override;

	void setScene(std::unique_ptr<Scene> scene) {
		this->scene = std::move(scene);
	}

	Scene* getScene() {
		return scene.get();
	}

private:
	std::unique_ptr<Scene> scene;
	float deltatime;
};

}


#endif// GIPECS_GSCENE_H
