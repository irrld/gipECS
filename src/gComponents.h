//
// Created by Metehan Gezer on 14/09/2025.
//

#ifndef GIPECS_GCOMPONENTS_H
#define GIPECS_GCOMPONENTS_H

#include "entt/entt.hpp"
#include "gCamera.h"
#include "gImage.h"
#include "gModel.h"
#include "gUUID.h"

#include <glm/glm.hpp>
#include <string>

struct gComponentBase {
	virtual ~gComponentBase() = default;
};

struct gIdComponent : public gComponentBase {
	gIdComponent(gUUID id) : Id(id) {}

	gIdComponent() = default;
	gIdComponent(const gIdComponent&) = default;

	gUUID Id;
};

struct gTreeComponent : public gComponentBase {
	gTreeComponent() = default;
	gTreeComponent(const gTreeComponent&) = default;

	entt::entity parent = entt::null;
	std::vector<entt::entity> childs;
};

struct gTagComponent : public gComponentBase {
	gTagComponent(const std::string& tag) : tag(tag) {}
	gTagComponent() = default;
	gTagComponent(const gTagComponent&) = default;

	std::string tag;
};


struct gTransformComponent : public gComponentBase {
	gTransformComponent() = default;
	gTransformComponent(const gTransformComponent&) = default;

	glm::vec3 getForward();
	glm::vec3 getBackward();
	glm::vec3 getLeft();
	glm::vec3 getRight();
	glm::vec3 getUp();
	glm::vec3 getDown();

	void move(float dx, float dy, float dz);

	void move(const glm::vec3& delta) { move(delta.x, delta.y, delta.z); }

	void setPosition(float x, float y, float z);

	void setPosition(glm::vec3& pos) { setPosition(pos.x, pos.y, pos.z); }

	void rotate(float dx, float dy, float dz);

	void rotate(const glm::vec3& delta) { rotate(delta.x, delta.y, delta.z); }

	void setRotation(float x, float y, float z);

	void setRotation(glm::vec3& rot) { setRotation(rot.x, rot.y, rot.z); }

	void resize(float dx, float dy, float dz);

	void resize(const glm::vec3& delta) { resize(delta.x, delta.y, delta.z); }

	void setScale(float x, float y, float z);

	void setScale(const glm::vec3& scale) { setScale(scale.x, scale.y, scale.z); }

	glm::vec3 position = {0.0f, 0.0f, 0.0f};
	glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
	glm::vec3 scale = {1.0f, 1.0f, 1.0f};
	glm::vec3 pivot = {0.0f, 0.0f, 0.0f};

	bool ischanged = true;
	glm::mat4 transformmatrix = {};
	glm::mat3 normalmatrix = {};
};

struct gSpriteComponent : public gComponentBase {
	gSpriteComponent() = default;
	gSpriteComponent(const gSpriteComponent&) = default;

	gImage image;
};

void loadSpriteComponent(gSpriteComponent& component, const std::string& imagepath);

struct gCameraComponent : public gComponentBase {
	gCameraComponent() = default;
	gCameraComponent(const gCameraComponent&) = default;

	gCamera camera;
};

struct gModelComponent : public gComponentBase {
	gModelComponent() = default;
	gModelComponent(const gModelComponent&) = default;

	gModel model;
};

#endif//GIPECS_GCOMPONENTS_H
