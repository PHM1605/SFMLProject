#pragma once
#include "Button.hpp"
#include "State.hpp"
#include "Container.hpp"
#include "Label.hpp"
#include <array>

class SettingsState : public State {
public:
public:
	SettingsState(StateStack& stack, Context context);
	virtual void draw();
	virtual bool update(sf::Time dt);
	virtual bool handleEvent(const sf::Event& event);
private:
	sf::Sprite mBackgroundSprite;
	GUI::Container mGUIContainer;
	std::array<GUI::Button::Ptr, Player::ActionCount> mBindingButtons;
	std::array<GUI::Label::Ptr, Player::ActionCount> mBindingLabels;
private:
	void updateLabels();
	void addButtonLabel(Player::Action action, float y, const std::string& text, Context context);
};
