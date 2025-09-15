//
// Created by Metehan Gezer on 14/09/2025.
//

#ifndef GIPECS_GCOMPONENTS_H
#define GIPECS_GCOMPONENTS_H

#include "entt/entt.hpp"
#include "gCamera.h"
#include "gFbo.h"
#include "gImage.h"
#include "gLight.h"
#include "gModel.h"
#include "gUUID.h"

#include <glm/glm.hpp>
#include <string>

namespace gecs {

struct gComponentBase {
	virtual ~gComponentBase() = default;
};

struct IdComponent : public gComponentBase {
	IdComponent(gUUID id) : id(id) {}

	IdComponent() = default;
	IdComponent(const IdComponent&) = default;

	gUUID id;
};

struct TreeComponent : public gComponentBase {
	TreeComponent() = default;
	TreeComponent(const TreeComponent&) = default;

	entt::entity parent = entt::null;
	std::vector<entt::entity> childs;
};

struct TagComponent : public gComponentBase {
	TagComponent(const std::string& tag) : tag(tag) {}
	TagComponent() = default;
	TagComponent(const TagComponent&) = default;

	std::string tag;
};

struct TransformComponent : public gComponentBase {
	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;

	glm::vec3 getForward();
	glm::vec3 getBackward();
	glm::vec3 getLeft();
	glm::vec3 getRight();
	glm::vec3 getUp();
	glm::vec3 getDown();

	void move(float dx, float dy, float dz);

	void move(const glm::vec3& delta) { move(delta.x, delta.y, delta.z); }

	void setPosition(float x, float y, float z);

	void setPosition(const glm::vec3& pos) { setPosition(pos.x, pos.y, pos.z); }

	void rotate(float dx, float dy, float dz);

	void rotate(const glm::vec3& delta) { rotate(delta.x, delta.y, delta.z); }

	void setRotation(float x, float y, float z);

	void setRotation(const glm::vec3& rot) { setRotation(rot.x, rot.y, rot.z); }

	void resize(float dx, float dy, float dz);

	void resize(const glm::vec3& delta) { resize(delta.x, delta.y, delta.z); }

	void setScale(float x, float y, float z);

	void setScale(const glm::vec3& scale) { setScale(scale.x, scale.y, scale.z); }

	void lookAt(const glm::vec3& target) { lookAt(target.x, target.y, target.z); }

	void lookAt(float x, float y, float z) {
		glm::vec3 to = glm::vec3(x, y, z) - position;
		float len2 = glm::dot(to, to);
		if (len2 < 1e-12f) return; // same point

		glm::vec3 d = glm::normalize(to);

		// Right-handed, -Z forward system:
		float yaw   = std::atan2(d.x, -d.z);  // Rotation around Y-axis
		float pitch = std::asin(glm::clamp(d.y, -1.0f, 1.0f)); // Rotation around X-axis
		rotation = glm::vec3(pitch, yaw, 0.0f);

		glm::quat lookorientation = glm::conjugate(glm::quat_cast(glm::lookAt(position, glm::vec3(x, y, z), glm::vec3(0.0f, 1.0f, 0.0f))));
		rotation = glm::eulerAngles(lookorientation);
		ischanged = true;
	}

	glm::vec3 position = {0.0f, 0.0f, 0.0f};
	// rotation is in degrees
	glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
	glm::vec3 scale = {1.0f, 1.0f, 1.0f};
	glm::vec3 pivot = {0.0f, 0.0f, 0.0f};

	bool ischanged = true;
	glm::mat4 transformmatrix = {};
	glm::mat3 normalmatrix = {};
};

struct SpriteComponent : public gComponentBase {
	SpriteComponent() = default;
	SpriteComponent(const SpriteComponent&) = default;

private:
	friend class Scene;
	friend class Loader;

	gImage data;
};

class Camera : public gCamera {
public:
	void setComponentsUnsafe(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, glm::mat4 matrix) {
		position = pos;
		orientation = glm::quatLookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::angleAxis(rot.x, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::angleAxis(rot.z, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::angleAxis(rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
		scalevec = scale;
		lookposition = position;
		lookorientation = orientation;
		lookscalevec = scalevec;
		locallookmatrix = matrix;
		localtransformationmatrix = matrix;
	}

	void updateFrustum(glm::vec3 position, glm::mat4 mat, float nearplane, float farplane, float aspectratio, float fovradians) {
		const float halfVSide = farplane * tanf(fovradians * 0.5f);
		const float halfHSide = halfVSide * aspectratio;
		glm::vec3 front = -mat[2];
		glm::vec3 right = -mat[0];
		glm::vec3 up = -mat[1];
		const glm::vec3 frontMultFar = farplane * front;
		frustum.nearFace = Plane{ position + nearplane * front, front };
		frustum.farFace = Plane{ position + frontMultFar, -front };
		frustum.rightFace = Plane{ position, glm::cross(frontMultFar - right * halfHSide, up) };
		frustum.leftFace = Plane{ position, glm::cross(up, frontMultFar + right * halfHSide) };
		frustum.topFace = Plane{ position, glm::cross(right, frontMultFar - up * halfVSide) };
		frustum.bottomFace = Plane{ position, glm::cross(frontMultFar + up * halfVSide, right) };
	}
};

struct CameraComponent : public gComponentBase {
	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;

	float fov = 60.0f;
	float nearplane = 0.01f;
	float farplane = 1000.0f;
	float aspectratio = 1.0f;
	bool isenabled = true;

private:
	friend class Scene;
	friend class Loader;

	void begin(glm::vec3 position, glm::mat4 transformationmatrix);
	void end();

	Camera data;
	gFbo fbo;
};

struct LightAmbientComponent : public gComponentBase {
	LightAmbientComponent() = default;
	LightAmbientComponent(const LightAmbientComponent&) = default;

	bool isenabled;
private:
	friend class Scene;
	friend class Loader;

	gLight data{gLight::LIGHTTYPE_AMBIENT};
};

struct ModelComponent : public gComponentBase {
	ModelComponent() = default;
	ModelComponent(const ModelComponent&) = default;

private:
	friend class Scene;
	friend class Loader;

	gModel data;
};

}


#endif//GIPECS_GCOMPONENTS_H
