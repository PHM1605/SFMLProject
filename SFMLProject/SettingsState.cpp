#include "SettingsState.hpp"

SettingsState::SettingsState(StateStack& stack, Context context) :
	State(stack, context),
	mGUIContainer()
{
	mBackgroundSprite.setTexture(context.textures->get(Textures::TitleScreen));

	addButtonLabel(Player::MoveLeft, 150.f, "Move Left", context);
	addButtonLabel(Player::MoveRight, 200.f, "Move Right", context);
	addButtonLabel(Player::MoveUp, 250.f, "Move Up", context);
	addButtonLabel(Player::MoveDown, 300.f, "Move Down", context);

	updateLabels();
}

void SettingsState::updateLabels() {

}

void SettingsState::addButtonLabel(Player::Action action, float y, const std::string& text, Context context) {
	mBindingButtons[action] = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
	mBindingButtons[action]->setPosition(80.f, y);
	mBindingButtons[action]->setText(text);
	mBindingButtons[action]->setToggle(true);

	mBindingLabels[action] = std::make_shared<GUI::Label>("", *context.fonts);
	mBindingLabels[action]->setPosition(300.f, y + 15.f);

	mGUIContainer.pack(mBindingButtons[action]);
	mGUIContainer.pack(mBindingLabels[action]);
}