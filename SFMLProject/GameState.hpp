#pragma once
#include "State.hpp"
#include "StateStack.hpp"
#include "World.hpp"

class GameState : public State {
public:
	GameState(StateStack& stack, Context context);
	virtual void draw();
	virtual bool update(sf::Time dt);
	virtual bool handleEvent(const sf::Event& event);
private:
	World mWorld;
	Player& mPlayer;
};