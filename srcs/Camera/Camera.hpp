#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Inputs/Inputs.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class Camera {
	public:
	glm::vec3 velocity {0.f, 0.f, 0.f};
	glm::vec3 position {5.f, 1.f, 5.f};

	float pitch { 0.0f };
	float yaw { 0.0f };

	glm::mat4 getViewMatrix();
	glm::mat4 getRotationMatrix();

	void processInput(Input *instance);

	void update();
};