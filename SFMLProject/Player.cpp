#include "Player.hpp"
#include "Aircraft.hpp"
#include <iostream>

// Functor = function object
// Use for "Accelerate" events
struct AircraftMover {
	sf::Vector2f velocity;
	AircraftMover(float vx, float vy) : velocity(vx, vy) {} // construtor
	void operator() (Aircraft& aircraft, sf::Time) const {
		aircraft.accelerate(velocity);
	}
};

// casting GameObject e.g. Aircraft to SceneNode to be put on Queue
template<typename GameObject, typename Function>
std::function<void(SceneNode&, sf::Time)> derivedAction(Function fn) {
	return [=](SceneNode& node, sf::Time dt) {
		assert(dynamic_cast<GameObject*>(&node) != nullptr);
		fn(static_cast<GameObject&>(node), dt);
	};
}

Player::Player() {
	mKeyBinding[sf::Keyboard::Left] = MoveLeft;
	mKeyBinding[sf::Keyboard::Right] = MoveRight;
	mKeyBinding[sf::Keyboard::Up] = MoveUp;
	mKeyBinding[sf::Keyboard::Down] = MoveDown;
	initializeActions();
	for (auto& pair: mActionBinding) {
		pair.second.category = Category::PlayerAircraft;
	}
	
}

void Player::initializeActions() {
	const float playerSpeed = 200.f;
	mActionBinding[MoveLeft].action = derivedAction<Aircraft>(AircraftMover(-playerSpeed, 0.f));
	mActionBinding[MoveRight].action = derivedAction<Aircraft>(AircraftMover(playerSpeed, 0.f));
	mActionBinding[MoveUp].action = derivedAction<Aircraft>(AircraftMover(0.f, -playerSpeed));
	mActionBinding[MoveDown].action = derivedAction<Aircraft>(AircraftMover(0.f, playerSpeed));
}

void Player::handleRealtimeInput(CommandQueue& commands) {
	for (auto pair : mKeyBinding) {
		if (sf::Keyboard::isKeyPressed(pair.first) && isRealtimeAction(pair.second))
			commands.push(mActionBinding[pair.second]);
	}
}

// print aircraft position when press P
void Player::handleEvent(const sf::Event& event, CommandQueue& commands) {
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
		Command output;
		output.category = Category::PlayerAircraft;
		output.action = [](SceneNode& s, sf::Time) {
			std::cout << s.getPosition().x << "," << s.getPosition().y << "\n";
		};
		commands.push(output);
	}
}

bool Player::isRealtimeAction(Action action) {
	switch (action) {
	case MoveLeft:
	case MoveRight:
	case MoveDown:
	case MoveUp:
		return true;
	default:
		return false;
	}
}

// For Settings later to allow user to set their own keys
void Player::assignKey(Action action, sf::Keyboard::Key key) {
	for (auto itr = mKeyBinding.begin(); itr != mKeyBinding.end();) {
		if (itr->second == action)
			mKeyBinding.erase(itr++);
		else
			++itr;
	}
	mKeyBinding[key] = action;
}

sf::Keyboard::Key Player::getAssignedKey(Action action) const {
	for (auto pair : mKeyBinding) {
		if (pair.second == action)
			return pair.first;
	}
	return sf::Keyboard::Unknown;
}