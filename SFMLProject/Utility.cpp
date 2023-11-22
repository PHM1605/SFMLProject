#include "Utility.hpp"
#include <cmath>

// center of sprite is its origin
void centerOrigin(sf::Sprite& sprite) {
	sf::FloatRect bounds = sprite.getLocalBounds();
	sprite.setOrigin(std::floor(bounds.left + bounds.width / 2.f), std::floor(bounds.top + bounds.height / 2.f));
}

// center of text is its origin
void centerOrigin(sf::Text& text) {
	sf::FloatRect bounds = text.getLocalBounds();
	text.setOrigin(std::floor(bounds.left + bounds.width / 2.f), std::floor(bounds.top + bounds.height / 2.f));
}

float toDegree(float radian) {
	return 180.f / 3.141593f * radian;
}

float toRadian(float degree) {
	return 3.141593f / 180.f * degree;
}

float length(sf::Vector2f vector) {
	return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}

sf::Vector2f unitVector(sf::Vector2f vector) {
	assert(vector != sf::Vector2f(0.f, 0.f));
	return vector / length(vector);
}