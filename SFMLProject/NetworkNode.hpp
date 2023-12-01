#pragma once
#include "SceneNode.hpp"
#include "NetworkProtocol.hpp"
#include <queue>

class NetworkNode : public SceneNode {
public:
	NetworkNode();
	void notifyGameAction(GameActions::Type type, sf::Vector2f position); // push Action to the queue of mPendingActions
	bool pollGameAction(GameActions::Action& out); // take action out and store it in "out" (bool to indicate there is pendingAction or not in the queue)
	virtual unsigned int getCategory() const; // Category::Network
private:
	std::queue<GameActions::Action> mPendingActions;
};