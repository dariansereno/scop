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
	static void setupInput(GLFWwindow * window);

	private:
	static void callback(GLFWwindow * window, int key, int scancode, int action, int mods);

	std::map<int, bool> _keys;
	static std::vector<Input *> _instances;
};
