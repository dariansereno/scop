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

void Input::setupInput(Input *instance, GLFWwindow * window) {
	glfwSetKeyCallback(window, Input::callbackKey);
	glfwSetMouseButtonCallback(window, Input::callbackMouse);
	glfwSetCursorPosCallback(window, Input::callbackCursor);
	glfwSetWindowUserPointer(window, instance);
}

void Input::callbackKey(GLFWwindow * window, int key, int scancode, int action, int mods) {
	for (auto &instance: _instances ) {
		instance->setIsKeyDown(key, action != GLFW_RELEASE);
	}
}

void Input::callbackMouse(GLFWwindow* window, int button, int action, int mods) {
    Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));

    if (input) {
        input->setIsMouseButtonDown(button, action != GLFW_RELEASE);
    }
}

void Input::callbackCursor(GLFWwindow* window, double xpos, double ypos) {
    Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));

    if (input) {
        input->_mouseX = xpos;
        input->_mouseY = ypos;
        input->_prevMouseX = input->_mouseX;
        input->_prevMouseY = input->_mouseY;
    }
}

bool Input::isMouseButtonDown(int button) {
    auto it = _mouseButtons.find(button);
    if (it != _mouseButtons.end()) {
        return it->second;
    }
    return false;
}

void Input::setIsMouseButtonDown(int button, bool isDown) {
    _mouseButtons[button] = isDown;
}

double Input::getMouseX() {
    return _mouseX;
}

double Input::getMouseY() {
    return _mouseY;
}

double Input::getMouseDeltaX() {
    return _mouseX - _prevMouseX;
}

double Input::getMouseDeltaY() {
    return _mouseY - _prevMouseY;
}
