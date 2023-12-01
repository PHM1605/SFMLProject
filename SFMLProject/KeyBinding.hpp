#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

namespace PlayerAction {
	enum Type {MoveLeft, MoveRight, MoveUp, MoveDown, Fire, LaunchMissile};
}

class KeyBinding {
public:
	typedef PlayerAction::Type Action;
public:
	// set initial Key config for Player1/2 (it is the value of controlPreconfiguration)
	explicit KeyBinding(int controlPreconfiguration);
	void assignKey(Action action, sf::Keyboard::Key key);
	sf::Keyboard::Key getAssignedKey(Action action) const;
	bool checkAction(sf::Keyboard::Key, Action& out) const; // Check if a Key on keyboard matched with an action->assign that Action to "out"
	std::vector<Action> getRealtimeActions() const;
private:
	std::map<sf::Keyboard::Key, Action> mKeyMap;
};

bool isRealtimeAction(PlayerAction::Type action);