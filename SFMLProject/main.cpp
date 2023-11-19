#include <SFML/Graphics.hpp>
#include <iostream>
#include "Application.hpp"

int main() {
	try {
		Application app;
		app.run();
	}
	catch (std::runtime_error& e) {
		std::cout << "Exception: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}