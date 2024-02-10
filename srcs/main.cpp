#include "Scene.hpp"
#include "Parser.hpp"
#include "WindowHandler.hpp"
#include "App.hpp"

int main(int ac, char **av) {
    if (ac != 2)
		return (1);

	Scene	scene;
	Parser	parser(av[1]);

	parser.parseObj(scene);
	auto indices = scene.getIndices();
	for (auto it = indices.begin(); it != indices.end(); it++) {
		std::cout << *it << std::endl;
	}
	 App app;

	try {
		app.run(scene);

	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

//int main() {
//    HelloTriangleApplication app;

//    try {
//        app.run();
//    } catch (const std::exception& e) {
//        std::cerr << e.what() << std::endl;
//        return EXIT_FAILURE;
//    }

//    return EXIT_SUCCESS;
//}