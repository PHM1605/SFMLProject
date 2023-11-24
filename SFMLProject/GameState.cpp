#include "GameState.hpp"


GameState::GameState(StateStack& stack, Context context):
	State(stack, context),
	mWorld(*context.window, *context.fonts),
	mPlayer(*context.player)
{}

void GameState::draw() {
	mWorld.draw();
}

bool GameState::update(sf::Time dt) {
	mWorld.update(dt);
	if (!mWorld.hasAlivePlayer()) {
		mPlayer.setMissionStatus(Player::MissionFailure);

	}
	CommandQueue& commands = mWorld.getCommandQueue();
	mPlayer.handleRealtimeInput(commands);
	return true;
}

bool GameState::handleEvent(const sf::Event& event) {
	CommandQueue& commands = mWorld.getCommandQueue();
	mPlayer.handleEvent(event, commands);
	// ESCAPE press, enter the Pause Screen
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		requestStackPush(States::Pause);
	return true;
}