#include "Camera.hpp"
#include <iostream>

glm::mat4 Camera::getViewMatrix() {
	glm::mat4 translation = glm::translate(glm::mat4(1.f), position);
	glm::mat4 rotation = getRotationMatrix();
	return glm::inverse(translation * rotation);
}
glm::mat4 Camera::getRotationMatrix() {
	glm::quat pitchRotation = glm::angleAxis(pitch, glm::vec3{1.0f, 0.f, 0.f});
	glm::quat yawRotation = glm::angleAxis(yaw, glm::vec3{0.0f, -1.0f, 0.0f});

	return glm::toMat4(yawRotation) * glm::toMat4(pitchRotation);
}

void Camera::processInput(Input *instance) {
	// Key down
	if (instance->isKeyDown(GLFW_KEY_D)) {velocity.x = 1.f;};
	if (instance->isKeyDown(GLFW_KEY_A)) velocity.x = -1.f;
	if (instance->isKeyDown(GLFW_KEY_W)) velocity.z = -1.f;
	if (instance->isKeyDown(GLFW_KEY_S)) velocity.z = 1.f;

	// Key up
	if (!instance->isKeyDown(GLFW_KEY_D)) velocity.x = 0.f;
	if (!instance->isKeyDown(GLFW_KEY_A)) velocity.x = 0.f;
	if (!instance->isKeyDown(GLFW_KEY_W)) velocity.z = 0.f;
	if (!instance->isKeyDown(GLFW_KEY_S)) velocity.z = 0.f;

	if (instance->getMouseDeltaX() > 0 || instance->getMouseDeltaY() > 0) {
		yaw += instance->getMouseDeltaX() / 200.f;
		pitch -= instance->getMouseDeltaY() / 200.f;
	}
}

void Camera::update() {
	glm::mat4 rotation = getRotationMatrix();
	position += glm::vec3(rotation * glm::vec4(velocity, 0.f));
}