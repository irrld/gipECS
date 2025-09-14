//
// Created by Metehan Gezer on 14/09/2025.
//

#ifndef GIPECS_GSCENE_H
#define GIPECS_GSCENE_H

#include "entt/entt.hpp"
#include "gBaseCanvas.h"
#include "gComponents.h"
#include "gSystem.h"

enum class gSystem {
	TICK,
	DRAW
};

class gEntity;

class gScene : gRenderObject{
public:
	gScene();
	~gScene();

	gEntity createEntity(const std::string& name = std::string());
	gEntity createEntityWithUUID(gUUID uuid,
								 const std::string& name = std::string());
	void removeEntity(gEntity entity);

	void onUpdate(float_t deltatime);
	void onEvent(gEvent& event);

	template<typename T>
	void onRemoveComponent(entt::entity entity, T& component) {}

	template<typename T>
	void onAddComponent(entt::entity entity, T& component) {}

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
	T& getComponent(entt::entity handle) {// This function is intentionally not marked as const!
		return registry.get<T>(handle);
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
	void bindSystem(gSystem type, Func func) {
		// Create a TypedSystemFunction with explicit component types
		TypedSystemFunction<Entity, gScene, Components...> systemfunc(func);

		if (type == gSystem::TICK) {
			tickfilters.emplace_back([systemfunc, this](entt::entity entity, entt::registry& reg) mutable {
				return systemfunc.hasComponents(entity, reg);
			});
			ticksystems.emplace_back([systemfunc, this](float dt, entt::entity entity, entt::registry& reg) mutable {
				systemfunc(this, dt, Entity{entity, this}, reg);
			});
		} else if (type == gSystem::DRAW) {
			drawfilters.emplace_back([systemfunc, this](entt::entity entity, entt::registry& reg) mutable {
				return systemfunc.hasComponents(entity, reg);
			});
			drawsystems.emplace_back([systemfunc, this](float dt, entt::entity entity, entt::registry& reg) mutable {
				systemfunc(this, dt, Entity{entity, this}, reg);
			});
		}
	}

	void tick(float deltatime);
	void draw(float deltatime);

private:
	bool onWindowResizeEvent(gWindowResizeEvent& event);
	glm::mat4 makeLocal(const gTransformComponent& transform);
	glm::mat4 getWorldMatrix(entt::entity entity);
	void updateMatrices(entt::entity entity);
	void destroyEntity(entt::entity handle);

	void updateTransform(float deltatime, gEntity entity, gTransformComponent& transform);
	void renderSprite(float deltatime, gEntity entity, gTransformComponent& transform, gSpriteComponent& sprite);

private:
	friend class gSceneCanvas;

	std::unordered_map<gUUID, entt::entity> entities;
	entt::registry registry;
	bool firstupdate = true;
	std::vector<entt::entity> scenehierarchy;
	std::vector<entt::entity> destroyqueue;
	std::vector<std::function<void(float, entt::entity, entt::registry&)>> ticksystems;
	std::vector<std::function<void(float, entt::entity, entt::registry&)>> drawsystems;
	std::vector<std::function<bool(entt::entity, entt::registry&)>> tickfilters;
	std::vector<std::function<bool(entt::entity, entt::registry&)>> drawfilters;
};


class gSceneCanvas : public gBaseCanvas {
public:
	gSceneCanvas(gBaseApp*);
	~gSceneCanvas() override;
	void setup() override;
	void update() override;
	void draw() override;
	void onEvent(gEvent& event) override;

	void setScene(std::unique_ptr<gScene> scene) {
		this->scene = std::move(scene);
	}

	gScene* getScene() {
		return scene.get();
	}

private:
	std::unique_ptr<gScene> scene;
	float deltatime;
};

#endif// GIPECS_GSCENE_H
