#pragma once
#include <map>
#include <vector>
#include <algorithm>
#include <GLFW/glfw3.h>

class Input {
	public:
	Input(std::vector<int> keys);
	~Input();

	bool isKeyDown(int key);
	void setIsKeyDown(int key, bool isDown);

	bool isMouseButtonDown(int button);
    void setIsMouseButtonDown(int button, bool isDown);
    double getMouseX();
    double getMouseY();
	double getMouseDeltaX();
	double getMouseDeltaY();

	static void setupInput(Input *instance, GLFWwindow * window);

	private:
	static void callbackKey(GLFWwindow * window, int key, int scancode, int action, int mods);
	static void callbackMouse(GLFWwindow* window, int button, int action, int mods);
	static void callbackCursor(GLFWwindow* window, double xpos, double ypos);

	std::map<int, bool> _keys;
    std::map<int, bool> _mouseButtons;
    double _mouseX;
    double _mouseY;
	double _prevMouseX;
	double _prevMouseY;
	static std::vector<Input *> _instances;
};
