#pragma once
#include "State.hpp"
#include "Utility.hpp"
#include "Container.hpp"
#include "Button.hpp"

class PauseState : public State {
public:
	PauseState(StateStack& stack, Context context, bool letUpdatesThrough = false);
	~PauseState();
	virtual void draw();
	virtual bool update(sf::Time dt);
	virtual bool handleEvent(const sf::Event& event);
private:
	sf::Sprite mBackgroundSprite;
	sf::Text mPausedText;
	GUI::Container mGUIContainer;
	bool mLetUpdatesThrough; // depends on offline (false) or online (true) gaming (online never pauses -> always update)
};