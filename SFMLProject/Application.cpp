#include "Application.hpp"
#include "Utility.hpp"
#include "State.hpp"
#include "GameState.hpp"
#include "MultiplayerGameState.hpp"
#include "TitleState.hpp"
#include "MenuState.hpp"
#include "PauseState.hpp"
#include "SettingsState.hpp"
#include "GameOverState.hpp"

const sf::Time Application::timePerFrame = sf::seconds(1.f / 60.f);

Application::Application()
	: mWindow(sf::VideoMode(1024, 768), "Audio", sf::Style::Close),
	mTextures(),
	mFonts(),
	mMusic(),
	mSounds(),
	mKeyBinding1(1),
	mKeyBinding2(2),
	mStateStack(State::Context(mWindow, mTextures, mFonts, mMusic, mSounds, mKeyBinding1, mKeyBinding2)),
	mStatisticsText(),
	mStatisticsUpdateTime(),
	mStatisticsNumFrames(0)
{
	mWindow.setKeyRepeatEnabled(false);
	mWindow.setVerticalSyncEnabled(true);
	mFonts.load(Fonts::Main, "Media/Sansation.ttf");
	mTextures.load(Textures::TitleScreen, "Media/Textures/TitleScreen.png");
	mTextures.load(Textures::Buttons, "Media/Textures/Buttons.png");
	mStatisticsText.setFont(mFonts.get(Fonts::Main));
	mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(10u);
	registerStates();
	mStateStack.pushState(States::Title);
	mMusic.setVolume(25.f);
}

void Application::run() {
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen()) {
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > timePerFrame) {
			timeSinceLastUpdate -= timePerFrame;
			processInput();
			update(timePerFrame);
			if (mStateStack.isEmpty())
				mWindow.close();
		}
		updateStatistics(elapsedTime);
		render();
	}
}

void Application::processInput() {
	sf::Event event;
	while (mWindow.pollEvent(event)) {
		mStateStack.handleEvent(event);
		if (event.type == sf::Event::Closed)
			mWindow.close();
	}
}

void Application::update(sf::Time elapsedTime) {
	mStateStack.update(elapsedTime);
}

void Application::render() {
	mWindow.clear();
	mStateStack.draw();
	mWindow.setView(mWindow.getDefaultView());
	mWindow.draw(mStatisticsText);
	mWindow.display();
}

void Application::updateStatistics(sf::Time elapsedTime) {
	mStatisticsUpdateTime += elapsedTime;
	mStatisticsNumFrames += 1;
	if (mStatisticsUpdateTime >= sf::seconds(1.0f)) {
		mStatisticsText.setString("FPS = " + toString(mStatisticsNumFrames));
		mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsNumFrames = 0;
	}
}

void Application::registerStates() {
	mStateStack.registerState<TitleState>(States::Title);
	mStateStack.registerState<MenuState>(States::Menu);
	mStateStack.registerState<GameState>(States::Game);
	mStateStack.registerState<MultiplayerGameState>(States::HostGame, true); // use MultiplayerGameState constructor with last params of true
	mStateStack.registerState<MultiplayerGameState>(States::JoinGame, false); // use MultiplayerGameState constructor with last params of false
	mStateStack.registerState<PauseState>(States::Pause);
	mStateStack.registerState<PauseState>(States::NetworkPause, true);
	mStateStack.registerState<SettingsState>(States::Settings);
	mStateStack.registerState<GameOverState>(States::GameOver, "Mission Failed!");
	mStateStack.registerState<GameOverState>(States::MissionSuccess, "Mission Successful!");
}