#include "Player.hpp"
#include "Aircraft.hpp"

// Functor = function object
// Use for "Accelerate" events
struct AircraftMover {
	sf::Vector2f velocity;
	AircraftMover(float vx, float vy) : velocity(vx, vy) {} // construtor
	void operator() (SceneNode& node, sf::Time) const {
		Aircraft& aircraft = static_cast<Aircraft&>(node);
		aircraft.accelerate(velocity);
	}
};

//void Player::handleRealtimeInput(CommandQueue& commands) {
//	const float playerSpeed = 30.f;
//	//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
//
//}