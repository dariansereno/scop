#include "Inputs.hpp"

std::vector<Input*> Input::_instances;

Input::Input(std::vector<int> keys) {
	for (int key: keys) {
		_keys[key] = false;
	}
	_instances.push_back(this);
}

Input::~Input() {
	_instances.erase(std::remove(_instances.begin(), _instances.end(), this), _instances.end());
}

bool Input::isKeyDown(int key) {
	auto it = _keys.find(key);

	if (it != _keys.end())
		return (_keys[key]);
	return false;
}
void Input::setIsKeyDown(int key, bool isDown) {
	auto it = _keys.find(key);

	if (it != _keys.end())
		_keys[key] = isDown;
}

void Input::setupInput(GLFWwindow * window) {
	glfwSetKeyCallback(window, Input::callback);
}

void Input::callback(GLFWwindow * window, int key, int scancode, int action, int mods) {
	for (auto &instance: _instances ) {
		instance->setIsKeyDown(key, action != GLFW_RELEASE);
	}
}
