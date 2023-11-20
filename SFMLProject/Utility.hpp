#pragma once
#include<sstream>
#include <SFML/Graphics.hpp>

// std::to_string does not work on MinGW
template <typename T>
std::string toString(const T & value) {
	std::stringstream stream;
	stream << value;
	return stream.str();
}

void centerOrigin(sf::Sprite& sprite);
void centerOrigin(sf::Text& text);