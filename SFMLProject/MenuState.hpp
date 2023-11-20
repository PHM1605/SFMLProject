#pragma once
#include "State.hpp"
#include <SFML/Graphics.hpp>
#include "Utility.hpp"

class MenuState : public State {
public:
	MenuState(StateStack& stack, Context context);
	virtual void draw();
	virtual bool update(sf::Time dt);
	virtual bool handleEvent(const sf::Event& event);
	void updateOptionText();
private:
	enum OptionNames {
		Play, Exit
	};
	sf::Sprite mBackgroundSprite;
	std::vector<sf::Text> mOptions;
	std::size_t mOptionIndex;
};