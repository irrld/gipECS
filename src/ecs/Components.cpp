//
// Created by Metehan Gezer on 14/09/2025.
//

#include "Components.h"

#include "gTracy.h"

namespace gecs {

void CameraComponent::begin(glm::vec3 position, glm::mat4 transformationmatrix) {
	G_PROFILE_ZONE_SCOPED_N("CameraComponent::begin()");
	gRenderer* renderer = gRenderObject::getRenderer();
	renderer->backupMatrices();
	float fovY = glm::radians(fov);
	aspectratio = fbo.getWidth() / static_cast<float>(fbo.getHeight());
	renderer->setProjectionMatrix(glm::perspective(fovY, aspectratio, nearplane, farplane));
	renderer->setViewMatrix(glm::inverse(transformationmatrix));
	renderer->setCameraPosition(position);
	renderer->setCamera(&data);
	if(renderer->isGridEnabled()) renderer->drawGrid();

	data.updateFrustum(position, transformationmatrix, nearplane, farplane, aspectratio, fovY);
	renderer->updateScene();
}

void CameraComponent::end() {
	G_PROFILE_ZONE_SCOPED_N("CameraComponent::end()");
	gRenderer* renderer = gRenderObject::getRenderer();
	renderer->restoreMatrices();
	renderer->setCamera(nullptr);
	renderer->updateScene();
}

glm::vec3 TransformComponent::getForward() {
	return -transformmatrix[2];
}

glm::vec3 TransformComponent::getBackward() {
	return transformmatrix[2];
}

glm::vec3 TransformComponent::getLeft() {
	return -transformmatrix[0];
}

glm::vec3 TransformComponent::getRight() {
	return transformmatrix[0];
}

glm::vec3 TransformComponent::getUp() {
	return transformmatrix[1];
}

glm::vec3 TransformComponent::getDown() {
	return -transformmatrix[1];
}

void TransformComponent::move(float dx, float dy, float dz) {
	position += glm::vec3{dx, dy, dz};
	ischanged = true;
}

void TransformComponent::setPosition(float x, float y, float z) {
	position = glm::vec3{x, y, z};
	ischanged = true;
}

void TransformComponent::rotate(float dx, float dy, float dz) {
	rotation += glm::vec3{dx, dy, dz};
	ischanged = true;
}

void TransformComponent::setRotation(float x, float y, float z) {
	rotation = glm::vec3{x, y, z};
	ischanged = true;
}

void TransformComponent::resize(float dx, float dy, float dz) {
	scale += glm::vec3{dx, dy, dz};
	ischanged = true;
}

void TransformComponent::setScale(float x, float y, float z) {
	scale = glm::vec3{x, y, z};
	ischanged = true;
}
}
