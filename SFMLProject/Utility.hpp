#pragma once
#include<sstream>
#include <cassert>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <random>
#include "Animation.hpp"

// std::to_string does not work on MinGW
template <typename T>
std::string toString(const T & value) {
	std::stringstream stream;
	stream << value;
	return stream.str();
}

void centerOrigin(sf::Sprite& sprite);
void centerOrigin(sf::Text& text);
void centerOrigin(Animation& animation);

float toDegree(float radian);
float toRadian(float degree);

float length(sf::Vector2f vector);
sf::Vector2f unitVector(sf::Vector2f vector);
int randomInt(int exclusiveMax);