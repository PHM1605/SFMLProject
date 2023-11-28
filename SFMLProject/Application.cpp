#include "Application.hpp"
#include "GameState.hpp"
#include "TitleState.hpp"
#include "MenuState.hpp"
#include "Utility.hpp"
#include "PauseState.hpp"
#include "SettingsState.hpp"
#include "GameOverState.hpp"

const sf::Time Application::timePerFrame = sf::seconds(1.f / 60.f);

Application::Application()
	: mWindow(sf::VideoMode(640, 480), "Gameplay", sf::Style::Close),
	mTextures(),
	mFonts(),
	mPlayer(),
	mMusic(),
	mSounds(),
	mStateStack(State::Context(mWindow, mTextures, mFonts, mPlayer, mMusic, mSounds)),
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
	mStateStack.registerState<PauseState>(States::Pause);
	mStateStack.registerState<SettingsState>(States::Settings);
	mStateStack.registerState<GameOverState>(States::GameOver);
}