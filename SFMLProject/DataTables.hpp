#pragma once
#include "ResourceIdentifiers.hpp"
#include "Aircraft.hpp"

struct Direction {
	Direction(float angle, float distance):
		angle(angle),
		distance(distance)
	{}
	float angle;
	float distance;
};

struct AircraftData {
	int hitpoints;
	float speed;
	Textures::ID texture;
	sf::Time fireInterval;
	std::vector<Direction> directions;
};

std::vector<AircraftData> initializeAircraftData();