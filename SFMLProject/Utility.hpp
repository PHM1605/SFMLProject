#pragma once
#include<sstream>
#include <cassert>
#include <cmath>
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

float toDegree(float radian);
float toRadian(float degree);

sf::Vector2f unitVector(sf::Vector2f vector);